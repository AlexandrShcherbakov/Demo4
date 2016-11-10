#include <cmath>
#include <cstdlib>
#include <iostream>

#include "Utility.h"
#include "HydraExport.h"
#include "GeometryLib\Octree.h"

using namespace std;

HydraGeomData hyFile;
vector<VM::vec4> points, normals;
vector<VM::vec2> texCoords;
vector<uint> materialNum, indices;

map<uint, vector<uint> > splitedIndices;

map<uint, GL::Buffer*> indicesBuffers;
GL::Buffer *pointsBuffer, *normalsBuffer, *texCoordsBuffer;

GL::RWTexture * shadowMap;
map<uint, GL::Texture*> textures;
map<uint, GL::Material> materials;
map<uint, VM::vec4> ambientColor;

GL::ShaderProgram *texturedShader, *coloredShader;
GL::ShaderProgram *shadowMapShader;

map<uint, GL::Mesh*> meshes;

GL::Mesh * fullGeometry;

GL::SpotLightSource light;

GL::Camera camera;

Octree scene_space;

CL::Program program;

CL::Kernel compressor, compute_emission;

CL::Buffer formfactors_big, formfactors, rand_coords, polygons_geometry;
CL::Buffer light_matrix, light_params, shadow_map_buffer, emission;

bool CreateFF = true;

void RenderLayouts() {
    //Render shadow
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fullGeometry->Draw(points.size(), shadowMap);

	//Count radiosity
    light_matrix.loadData(light.getMatrix().data().data());
    //compute_emission.run(1250);

	//Render scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto &it: meshes) {
        it.second->DrawWithIndices();
	}
	glutSwapBuffers();
}

void FinishProgram() {
    glutDestroyWindow(glutGetWindow());
}

void KeyboardEvents(unsigned char key, int x, int y) {
	if (key == 27) {
		FinishProgram();
	} else if (key == 'w') {
		camera.goForward();
	} else if (key == 's') {
		camera.goBack();
	}
}

void SpecialButtons(int key, int x, int y) {
	if (key == GLUT_KEY_RIGHT) {
	} else if (key == GLUT_KEY_LEFT) {
	} else if (key == GLUT_KEY_UP) {
        camera.goForward();
	} else if (key == GLUT_KEY_DOWN) {
		camera.goBack();
	}
}

void IdleFunc() {
    glutPostRedisplay();
}

void MouseMove(int x, int y) {
    const static float centerX = 400.0f, centerY = 300.0f;
    if (x != 400 || y != 300) {
		camera.rotateY((x - centerX) / 1000.0f);
		camera.rotateTop((y - centerY) / 1000.0f);
		glutWarpPointer(400, 300);
    }
}

void MouseClick(int button, int state, int x, int y) {
}

void InitializeGLUT(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitContextVersion(3, 0);
	glutInitWindowPosition(-1, -1);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Demo 4");
	glutWarpPointer(400, 300);
	glutSetCursor(GLUT_CURSOR_NONE);

	glutDisplayFunc(RenderLayouts);
    glutKeyboardFunc(KeyboardEvents);
    glutSpecialFunc(SpecialButtons);
    glutIdleFunc(IdleFunc);
    glutPassiveMotionFunc(MouseMove);
    glutMouseFunc(MouseClick);
}

void ReadData(const string &path) {
    hyFile.read(path);
	for (uint i = 0; i < hyFile.getVerticesNumber(); i++) {
		points.push_back(VM::vec4(hyFile.getVertexPositionsFloat4Array() + 4 * i));
		normals.push_back(VM::vec4(hyFile.getVertexNormalsFloat4Array() + 4 * i));
		texCoords.push_back(VM::vec2(hyFile.getVertexTexcoordFloat2Array() + 2 * i));
        indices.push_back(hyFile.getTriangleVertexIndicesArray()[i]);
        materialNum.push_back(hyFile.getTriangleMaterialIndicesArray()[i / 3]);
	}
}

void ReadSplitedData(const string& path) {
    ifstream in(path, ios::in | ios::binary);
    uint size;
    in.read((char *)&size, sizeof(size));
    VM::vec4 point, normal;
    VM::vec2 tex_coord;
    uint mat_num;
    for (uint i = 0; i < size; ++i) {
        in.read((char*)&point, sizeof(point));
        points.push_back(point);
        in.read((char*)&normal, sizeof(normal));
        normals.push_back(normal);
        in.read((char*)&tex_coord, sizeof(tex_coord));
        texCoords.push_back(tex_coord);
        in.read((char*)&mat_num, sizeof(mat_num));
        materialNum.push_back(mat_num);
        indices.push_back(i);
    }
}

void ReadTestData(const string &path) {
    ifstream in(path);
    uint count;
    in >> count;
    float x, y, z, w;
    int index;
    for (uint i = 0; i < count; ++i) {
        in >> x >> y >> z >> w;
        points.push_back(VM::vec4(x, y, z, w));
        in >> x >> y >> z >> w;
        normals.push_back(VM::vec4(x, y, z, w));
        in >> x >> y;
        texCoords.push_back(VM::vec2(x, y));
        indices.push_back(i);
        in >> index;
        materialNum.push_back(index);
    }
}


void ReadMaterials(const string& path) {
	ifstream in(path);
    while(true) {
        string s;
        do {
            getline(in, s);
        } while (s.find("<material") == string::npos && !in.eof());
        if (in.eof())
            break;
        uint startInd = s.find("maxid = \"") + 9;
        uint endInd = s.find('\"', startInd);
        int ind;
        sscanf(s.substr(startInd, endInd - startInd).c_str(), "%d", &ind);
        do {
            getline(in, s);
        } while (s.find("<color>") == string::npos);
        VM::vec4 color;
        sscanf(s.substr(s.find("<color>") + 7).c_str(), "%f %f %f", &color.x, &color.y, &color.z);
        do {
            getline(in, s);
        } while (s.find("<texture>") == string::npos && s.find("</material>") == string::npos);
        GL::Texture * tex = nullptr;
        if (s.find("<texture>") != string::npos) {
			startInd = s.find("<texture> ") + 10;
			endInd = s.find(" </texture>");
			s = s.substr(startInd, endInd - startInd);
			tex = new GL::Texture(s);
			tex->setSlot(0);
        }
        materials[ind] = GL::Material(color, tex);
    }
}

void InitShadowMap() {
	shadowMap = new GL::RWTexture(2048, 2048);
	shadowMap->setSlot(1);
}

void SplitIndicesByMaterial() {
    for (uint i = 0; i < materialNum.size(); ++i)
        splitedIndices[materialNum[i]].push_back(indices[i]);
}

void CreateBuffers() {
    for (auto &it: splitedIndices) {
        indicesBuffers[it.first] = new GL::Buffer(GL_UNSIGNED_INT, GL_ELEMENT_ARRAY_BUFFER);
        indicesBuffers[it.first]->setData(it.second);
    }
    pointsBuffer = new GL::Buffer(GL_FLOAT, GL_ARRAY_BUFFER);
    normalsBuffer = new GL::Buffer(GL_FLOAT, GL_ARRAY_BUFFER);
    texCoordsBuffer = new GL::Buffer(GL_FLOAT, GL_ARRAY_BUFFER);
    pointsBuffer->setData(points);
    normalsBuffer->setData(normals);
    texCoordsBuffer->setData(texCoords);
}

void CreateMeshes() {
    for (auto &it: indicesBuffers) {
        meshes[it.first] = new GL::Mesh();
    }
    fullGeometry = new GL::Mesh();
}

void ReadShaders() {
    texturedShader = new GL::ShaderProgram("textured");
    coloredShader = new GL::ShaderProgram("colored");
    shadowMapShader = new GL::ShaderProgram("ShadowMap");
}

void AddBuffersToMeshes() {
    for (auto &it: meshes) {
        GL::ShaderProgram *shader;
        if (it.second->texturedMaterial()) {
            shader = texturedShader;
            it.second->bindBuffer(*texCoordsBuffer, *shader, "texCoord");
        } else {
            shader = coloredShader;
        }
        it.second->bindBuffer(*pointsBuffer, *shader, "points");
        it.second->bindBuffer(*normalsBuffer, *shader, "normal");
        it.second->bindIndicesBuffer(*indicesBuffers[it.first]);
    }
    fullGeometry->bindBuffer(*pointsBuffer, *shadowMapShader, "points");
}

void CreateLight() {
    light.innerCone = 5.0f / 180.0f * M_PI;
    light.angle = 35.0f / 180.0f * M_PI;
    light.position = VM::vec3(0.0f, 0.5f, 0.0f);
}

void CreateCamera() {
    camera.angle = 45.0f / 180.0f * M_PI;
    camera.direction = VM::vec3(0, 0, -1);
    camera.position = VM::vec3(0, 0.05, 0);
    camera.screenRatio = 800.0 / 600.0;
    camera.up = VM::vec3(0, 1, 0);
    camera.zfar = 50.0f;
    camera.znear = 0.05f;
}

void AddLightToShaders() {
    texturedShader->setLight("light", light);
    coloredShader->setLight("light", light);
}

void AddCameraToShaders() {

}

void AddLightToMeshes() {
    for (auto &it: meshes)
        it.second->addLight("light", light);
}

void AddCameraToMeshes() {
    for (auto &it: meshes)
        it.second->setCamera(&camera);
    fullGeometry->setCamera(&light);
}

void AddMaterialsToMeshes() {
    for (auto &it: materials) {
		if (meshes.find(it.first) != meshes.end())
			meshes[it.first]->setMaterial(it.second);
    }
}

void AddShadowMapToMeshes() {
    for (auto &it: meshes) {
        it.second->addTexture("shadowMap", *shadowMap);
    }
}

void AddShaderProgramToMeshes() {
    for (auto &it: meshes) {
        if (it.second->texturedMaterial())
            it.second->setShaderProgram(*coloredShader);
		else
			it.second->setShaderProgram(*texturedShader);
    }
    fullGeometry->setShaderProgram(*shadowMapShader);
}

void CreateCLProgram() {
    program.loadFromFile("kernels\\ker1.opencl");
}

void CreateCLKernels() {
    compressor = program.createKernel("Compress");
    compute_emission = program.createKernel("ComputeLightEmission");
}

void CreateCLBuffers() {
    rand_coords = program.createBuffer(CL_MEM_READ_ONLY, 20 * 2 * sizeof(float));
    formfactors_big = program.createBuffer(CL_MEM_READ_ONLY, sqr(1250) * sizeof(float));
    formfactors = program.createBuffer(CL_MEM_READ_WRITE, sqr(1250) * sizeof(float) / 2);
    polygons_geometry = program.createBufferFromGL(CL_MEM_READ_WRITE, pointsBuffer->getID());
    light_matrix = program.createBuffer(CL_MEM_READ_ONLY, 16 * sizeof(float));
    light_params = program.createBuffer(CL_MEM_READ_ONLY, 8 * sizeof(float));
    shadow_map_buffer = program.createBufferFromTexture(CL_MEM_READ_WRITE, 0, shadowMap->getID());
    emission = program.createBuffer(CL_MEM_READ_WRITE, points.size() / 3 * sizeof(VM::vec4) / 2);
}

vector<vector<float> > LoadMatrix(const string& filename) {
    vector<vector<float> > matrix;
    uint size;
    ifstream in(filename, ios::in | ios::binary);
    in.read((char*)&size, sizeof(size));
    matrix.resize(size);
    for (uint i = 0; i < size; ++i) {
        matrix[i].resize(size);
        for (uint j = 0; j < size; ++j) {
            in.read((char*)&matrix[i][j], sizeof(matrix[i][j]));
        }
    }
    return matrix;
}

vector<float> MatrixToVector(const vector<vector<float> >& matrix) {
    vector<float> result;
    for (uint i = 0; i < matrix.size(); ++i)
        for (uint j = 0; j < matrix[i].size(); ++j)
            result.push_back(matrix[i][j]);
    return result;
}

void FillCLBuffers() {
    vector<float> coords(40);
    for (uint i = 0; i < 20; ++i) {
        coords[2 * i] = (float) rand() / RAND_MAX;
        coords[2 * i + 1] = (float) rand() / RAND_MAX;
        float len = std::sqrt(sqr(coords[2 * i]) + sqr(coords[2 * i + 1]));
        coords[2 * i] /= len;
        coords[2 * i + 1] /= len;
    }
    rand_coords.loadData(coords.data());
    cout << "Random coords loaded" << endl;

    vector<float> ff = MatrixToVector(LoadMatrix("Precompute/ff20.bin"));
    formfactors_big.loadData(ff.data());
    compressor.addArgument(formfactors_big, 0);
    compressor.addArgument(formfactors, 1);
    compressor.run(ff.size());
    cout << "Form-factors loaded" << endl;

    vector<float> light_params_vec;
    light_params_vec.push_back(std::cos(5.0f / 180.0f * M_PI));
    light_params_vec.push_back(std::cos(35.0f / 180.0f * M_PI));
    for (uint i = 0; i < 3; ++i)
        light_params_vec.push_back(light.position[i]);
    for (uint i = 0; i < 3; ++i)
        light_params_vec.push_back(light.direction[i]);
    light_params.loadData(light_params_vec.data());
    cout << "Light parameters loaded" << endl;


}

void SetArgumentsForKernels() {
    //Compute emission
    compute_emission.addArgument(polygons_geometry, 0);
    compute_emission.addArgument(light_matrix, 1);
    compute_emission.addArgument(light_params, 2);
    compute_emission.addArgument(shadow_map_buffer, 3);
    compute_emission.addArgument(emission, 4);
    compute_emission.addArgument(rand_coords, 5);
    cout << "Arguments for emission computing added" << endl;
}

int main(int argc, char **argv) {
    cout << "Start" << endl;
    InitializeGLUT(argc, argv);
    cout << "GLUT inited" << endl;
	glewInit();
	cout << "glew inited" << endl;
	clewInit(L"OpenCL.dll");
	cout << "clew inited" << endl;
    //ReadData("Scenes/dabrovic-sponza/sponza_exported/scene.vsgf");
    //ReadTestData("Scenes/cornel box test/data.txt");
    //ReadSplitedData("Precompute/small_poly_sponza");
    ReadSplitedData("Precompute/New triangles");
    cout << "Data readed" << endl;
    ReadMaterials("Scenes\\dabrovic-sponza\\sponza_exported\\hydra_profile_generated.xml");
    cout << "Materials readed" << endl;
    InitShadowMap();
    cout << "ShadowMap inited" << endl;
    SplitIndicesByMaterial();
    cout << "Indices splited" << endl;
    CreateBuffers();
    cout << "Buffers created" << endl;
    CreateMeshes();
    cout << "Meshes created" << endl;
    ReadShaders();
    cout << "Shaders readed" << endl;
    AddBuffersToMeshes();
    cout << "Buffers added" << endl;
    CreateLight();
    cout << "Light source created" << endl;
    CreateCamera();
    cout << "Camera created" << endl;
    AddLightToShaders();
    cout << "Lights added to shaders" << endl;
    AddCameraToShaders();
    cout << "Camera added to shaders" << endl;
    AddLightToMeshes();
    cout << "Lights added to meshes" << endl;
    AddCameraToMeshes();
    cout << "Camera added to meshes" << endl;
    AddMaterialsToMeshes();
    cout << "Materials added to meshes" << endl;
    AddShadowMapToMeshes();
    cout << "ShadowMap added to meshes" << endl;
    AddShaderProgramToMeshes();
    cout << "Shader programs added to meshes" << endl;
    CreateCLProgram();
    cout << "CL program created" << endl;
    CreateCLKernels();
    cout << "CL kernels created" << endl;
    CreateCLBuffers();
    cout << "CL buffers created" << endl;
    FillCLBuffers();
    cout << "Fill CL buffers" << endl;
    SetArgumentsForKernels();
    cout << "Arguments added" << endl;
    glutMainLoop();
    return 0;
}
