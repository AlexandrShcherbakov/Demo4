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
vector<VM::vec4> relationWeights;
vector<VM::i16vec4> relationIndices;

vector<VM::vec4> ptcPoints;
vector<VM::vec4> ptcColors;
vector<VM::vec4> ptcNormals;

map<uint, vector<uint> > splitedIndices;

map<uint, GL::Buffer*> indicesBuffers;
GL::Buffer *pointsBuffer, *normalsBuffer, *texCoordsBuffer, *fullIndices;
GL::Buffer *indirectBuffer;

GL::RWTexture * shadowMap;
map<uint, GL::Texture*> textures;
map<uint, GL::Material> materials;
map<uint, VM::vec4> ambientColor;

GL::ShaderProgram *texturedShader, *coloredShader;
GL::ShaderProgram *shadowMapShader;

map<uint, GL::Mesh*> meshes;

GL::Mesh * fullGeometry;

GL::DirectionalLightSource light;

GL::Camera camera;

Octree scene_space;

vector<vector<pair<short, float> > > ff;
vector<uint> ffOffsetsVec;

CL::Program program;

CL::Kernel radiosity;
CL::Kernel computeIndirect, computeEmission;
CL::Kernel prepareBuffers;

CL::Buffer rand_coords;
CL::Buffer light_matrix, light_params, shadow_map_buffer, excident;
CL::Buffer ptcClrCL, ptcPointsCL, ptcNormalsCL;
CL::Buffer ffIndices, ffValues, ffOffsets, incident, indirect;
CL::Buffer indirectRelIndices, indirectRelWeights, pointsIncident;

bool CreateFF = true;
bool StartLightMove = false;

int radiosityIterations = 2;

void UpdateCLBuffers();

void SaveDirectLignt(const string& output) {
    ofstream out(output, ios::out | ios::binary);
    shared_ptr<float> data = excident->getData();
    out.write((char*)data.get(), sizeof(VM::vec4) * ptcColors.size());
    out.close();
    exit(0);
}

void SaveIndirectLignt(const string& output) {
    ofstream out(output, ios::out | ios::binary);
    shared_ptr<float> data = incident->getData();
    out.write((char*)data.get(), sizeof(VM::vec4) * ptcColors.size());
    out.close();
    exit(0);
}

void SaveFullIndirectLignt(const string& output) {
    ofstream out(output, ios::out | ios::binary);
    shared_ptr<float> data = indirect->getData();
    out.write((char*)data.get(), sizeof(VM::vec4) * ptcColors.size());
    out.close();
    exit(0);
}

void CountRadiosity() {
    UpdateCLBuffers();
    computeEmission->run(ptcColors.size());
    //SaveDirectLignt("lightning/emission20.bin");
    for (int i = 0; i < radiosityIterations; ++i) {
        radiosity->run(ptcColors.size());
        prepareBuffers->run(ptcColors.size());
    }
    //SaveDirectLignt("lightning/excident20x2.bin");
    computeIndirect->run(points.size());
}

void RenderLayouts() {
    UpdateCLBuffers();

    //Render shadow
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fullGeometry->DrawWithIndices(GL_TRIANGLES, shadowMap);

	//Count radiosity
	CountRadiosity();

	//Render scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	for (auto &it: meshes) {
        it.second->DrawWithIndices();
	}
	glutSwapBuffers();

    //light.position -= VM::vec3(0, 0, 0.005);
    if (StartLightMove) {
        light.direction = VM::normalize(light.direction + VM::vec3(0, 0, -0.005));
    }
}

void FreeResources() {
    radiosity = nullptr;
    computeIndirect = nullptr;
    computeEmission = nullptr;
    prepareBuffers = nullptr;

    rand_coords = nullptr;
    light_matrix = nullptr;
    light_params = nullptr;
    shadow_map_buffer = nullptr;
    ptcPointsCL = nullptr;
    ptcNormalsCL = nullptr;
    excident = nullptr;
    ptcClrCL = nullptr;
    ffIndices = nullptr;
    ffValues = nullptr;
    ffOffsets = nullptr;
    incident = nullptr;
    indirectRelIndices = nullptr;
    indirectRelWeights = nullptr;
    pointsIncident = nullptr;
    indirect = nullptr;
}

void FinishProgram() {
    FreeResources();
    glutDestroyWindow(glutGetWindow());
}

void KeyboardEvents(unsigned char key, int x, int y) {
	if (key == 27) {
		FinishProgram();
	} else if (key == 'w') {
		camera.goForward();
	} else if (key == 's') {
		camera.goBack();
	} else if (key == '+') {
        radiosityIterations++;
	} else if (key == '-') {
        radiosityIterations--;
	} else if (key == 'q') {
        StartLightMove = true;
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
	glutInitWindowPosition(40, 40);
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
    points.resize(size);
    normals.resize(size);
    texCoords.resize(size);
    materialNum.resize(size);
    relationIndices.resize(size);
    relationWeights.resize(size);
    for (uint i = 0; i < size; ++i) {
        in.read((char*)&points[i], sizeof(points[i]));
        in.read((char*)&normals[i], sizeof(normals[i]));
        in.read((char*)&texCoords[i], sizeof(texCoords[i]));
        in.read((char*)&materialNum[i], sizeof(materialNum[i]));
        in.read((char*)&relationIndices[i], sizeof(relationIndices[i]));
        in.read((char*)&relationWeights[i], sizeof(relationWeights[i]));

        if (materialNum[i] == 19) {
            texCoords[i].x = 1 - texCoords[i].x;
        }
    }
    uint indicesSize;
    in.read((char*)&indicesSize, sizeof(indicesSize));
    indices.resize(indicesSize);
    for (uint i = 0; i < indicesSize; ++i) {
        in.read((char*)&indices[i], sizeof(indices[i]));
    }
    in.close();
}

void ReadPatches(const string &input) {
    ifstream in(input, ios::in | ios::binary);
    uint size;
    in.read((char*)&size, sizeof(size));
    ptcPoints.resize(size * 4);
    ptcColors.resize(size);
    ptcNormals.resize(size);
    for (uint i = 0; i < size; ++i) {
        in.read((char*)&ptcColors[i], sizeof(ptcColors[i]));
        in.read((char*)&ptcNormals[i], sizeof(ptcNormals[i]));
        for (uint j = 0; j < 4; ++j) {
            in.read((char*)&ptcPoints[4 * i + j], sizeof(ptcPoints[4 * i + j]));
        }
    }
    in.close();
}

void ReadFormFactors(const string& input) {
    ifstream in(input, ios::in | ios::binary);
    short size;
    in.read((char*)&size, sizeof(size));
    ff.resize(size);
    ffOffsetsVec.resize(size + 1, 0);
    for (int i = 0; i < size; ++i) {
        short rowSize;
        in.read((char*)&rowSize, sizeof(rowSize));
        for (int j = 0; j < rowSize; ++j) {
            short index;
            float value;
            in.read((char*)&index, sizeof(index));
            in.read((char*)&value, sizeof(value));
            ff[i].push_back(make_pair(index, value));
        }
        ffOffsetsVec[i + 1] = ffOffsetsVec[i] + ff[i].size();
    }
    in.close();
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
    for (uint i = 0; i < indices.size(); ++i) {
        splitedIndices[materialNum[indices[i]]].push_back(indices[i]);
    }
}

void CreateBuffers() {
    for (auto &it: splitedIndices) {
        indicesBuffers[it.first] = new GL::Buffer(GL_UNSIGNED_INT, GL_ELEMENT_ARRAY_BUFFER);
        indicesBuffers[it.first]->setData(it.second);
    }
    pointsBuffer = new GL::Buffer(GL_FLOAT, GL_ARRAY_BUFFER);
    normalsBuffer = new GL::Buffer(GL_FLOAT, GL_ARRAY_BUFFER);
    texCoordsBuffer = new GL::Buffer(GL_FLOAT, GL_ARRAY_BUFFER);
    indirectBuffer = new GL::Buffer(GL_FLOAT, GL_ARRAY_BUFFER);
    fullIndices = new GL::Buffer(GL_UNSIGNED_INT, GL_ELEMENT_ARRAY_BUFFER);
    pointsBuffer->setData(points);
    normalsBuffer->setData(normals);
    texCoordsBuffer->setData(texCoords);
    indirectBuffer->setData(normals);
    fullIndices->setData(indices);
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
        it.second->bindBuffer(*indirectBuffer, *shader, "indirect");
        it.second->bindIndicesBuffer(*indicesBuffers[it.first]);
    }
    fullGeometry->bindBuffer(*pointsBuffer, *shadowMapShader, "points");
    fullGeometry->bindIndicesBuffer(*fullIndices);
}

void CreateLight() {
    light.SetInnerRadius(0.6096); //5.0f / 180.0f * M_PI;
    light.SetOutterRadius(0.6604); //35.0f / 180.0f * M_PI;
    light.position = VM::vec3(0.000838715, 0.491658, -0.123837);
    light.direction = VM::normalize(VM::vec3(-0.00145589, -0.98104, 0.193801));
    light.SetColor(VM::vec3(0.882353, 0.882353, 0.882353) * 4);
}

void CreateCamera() {
    camera.angle = 45.0f / 180.0f * M_PI;
    camera.position = VM::vec3(0.342602, 0.0575884, -0.0282203);
    camera.direction = -VM::normalize(VM::vec3(-97.7076, -3.95559, 19.2085) - camera.position);
    camera.screenRatio = 800.0 / 600.0;
    camera.up = VM::vec3(0, 1, 0);
    camera.zfar = 10000.0f;
    camera.znear = 0.001f;
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
    radiosity = program.createKernel("Radiosity");

    computeIndirect = program.createKernel("ComputeIndirect");

    computeEmission = program.createKernel("ComputeEmission");

    prepareBuffers = program.createKernel("PrepareBuffers");
}

void CreateCLBuffers() {
    rand_coords = program.createBuffer(CL_MEM_READ_ONLY, 20 * sizeof(VM::vec2));
    light_matrix = program.createBuffer(CL_MEM_READ_ONLY, 16 * sizeof(float));
    light_params = program.createBuffer(CL_MEM_READ_ONLY, 8 * sizeof(float));
    shadow_map_buffer = program.createBufferFromTexture(CL_MEM_READ_WRITE, 0, shadowMap->getID());
    ptcPointsCL = program.createBuffer(CL_MEM_READ_ONLY, ptcPoints.size() * sizeof(VM::vec4));
    ptcNormalsCL = program.createBuffer(CL_MEM_READ_ONLY, ptcNormals.size() * sizeof(VM::vec4));

    excident = program.createBuffer(CL_MEM_READ_WRITE, ptcColors.size() * sizeof(VM::vec4));
    ptcClrCL = program.createBuffer(CL_MEM_READ_ONLY, sizeof(VM::vec4) * ptcColors.size());

    ffIndices = program.createBuffer(CL_MEM_READ_ONLY, sizeof(short) * ffOffsetsVec.back());
    ffValues = program.createBuffer(CL_MEM_READ_ONLY, sizeof(float) * ffOffsetsVec.back());
    ffOffsets = program.createBuffer(CL_MEM_READ_ONLY, sizeof(uint) * ffOffsetsVec.size());
    incident = program.createBuffer(CL_MEM_READ_WRITE, sizeof(VM::vec4) * ptcColors.size());

    indirectRelIndices = program.createBuffer(CL_MEM_READ_ONLY, sizeof(VM::i16vec4) * relationIndices.size());
    indirectRelWeights = program.createBuffer(CL_MEM_READ_ONLY, sizeof(VM::vec4) * relationWeights.size());
    pointsIncident = program.createBufferFromGL(CL_MEM_READ_WRITE, indirectBuffer->getID());

    indirect = program.createBuffer(CL_MEM_READ_WRITE, sizeof(VM::vec4) * ptcColors.size());
}

void UpdateCLBuffers() {
    light_matrix->SetData(light.getMatrix().data().data());
    for (auto &it: meshes)
        it.second->addLight("light", light);
    fullGeometry->setCamera(&light);
}

void FillCLBuffers() {
    vector<float> coords(40);
    for (uint i = 0; i < 5; ++i) {
        coords[2 * i] = (float) rand() / RAND_MAX;
        coords[2 * i + 1] = (float) rand() / RAND_MAX;
        float len = std::sqrt(sqr(coords[2 * i]) + sqr(coords[2 * i + 1]));
        coords[2 * i] /= len;
        coords[2 * i + 1] /= len;
    }
    rand_coords->SetData(coords.data());
    cout << "Random coords loaded" << endl;

    vector<float> light_params_vec;
    light_params_vec.push_back(light.GetInnerRadius());
    light_params_vec.push_back(light.GetOutterRadius());
    for (uint i = 0; i < 3; ++i)
        light_params_vec.push_back(light.position[i]);
    for (uint i = 0; i < 3; ++i)
        light_params_vec.push_back(light.direction[i]);
    light_params->SetData(light_params_vec.data());
    cout << "Light parameters loaded" << endl;

    ptcClrCL->SetData(ptcColors.data());
    cout << "Patches colours loaded" << endl;

    vector<short> ffFullIndices;
    vector<float> ffFullValues;
    for (uint i = 0; i < ff.size(); ++i) {
        for (uint j = 0; j < ff[i].size(); ++j) {
            ffFullIndices.push_back(ff[i][j].first);
            ffFullValues.push_back(ff[i][j].second);
        }
    }

    ffIndices->SetData(ffFullIndices.data());
    cout << "Form-factors indices loaded" << endl;

    ffValues->SetData(ffFullValues.data());
    cout << "Form-factors values loaded" << endl;

    ffOffsets->SetData(ffOffsetsVec.data());
    cout << "Form-factors offsets loaded" << endl;

    indirectRelIndices->SetData(relationIndices.data());
    cout << "Indirect illumination relation indices loaded" << endl;

    indirectRelWeights->SetData(relationWeights.data());
    cout << "Indirect illumination relation weights loaded" << endl;

    ptcPointsCL->SetData(ptcPoints.data());
    cout << "Patches points loaded" << endl;

    ptcNormalsCL->SetData(ptcNormals.data());
    cout << "Patches normals loaded" << endl;
}

void SetArgumentsForKernels() {
    //Compute radiosity
    radiosity->addArgument(excident, 0);
    radiosity->addArgument(ffValues, 1);
    radiosity->addArgument(ffIndices, 2);
    radiosity->addArgument(ffOffsets, 3);
    radiosity->addArgument(ptcClrCL, 4);
    radiosity->addArgument(incident, 5);
    cout << "Arguments for radiosity added" << endl;

    //Compute indirect
    computeIndirect->addArgument(indirectRelIndices, 0);
    computeIndirect->addArgument(indirectRelWeights, 1);
    computeIndirect->addArgument(indirect, 2);
    computeIndirect->addArgument(pointsIncident, 3);
    cout << "Arguments for computing indirect added" << endl;

    computeEmission->addArgument(ptcPointsCL, 0);
    computeEmission->addArgument(rand_coords, 1);
    computeEmission->addArgument(light_matrix, 2);
    computeEmission->addArgument(light_params, 3);
    computeEmission->addArgument(shadow_map_buffer, 4);
    computeEmission->addArgument(ptcClrCL, 5);
    computeEmission->addArgument(excident, 6);
    computeEmission->addArgument(indirect, 7);
    computeEmission->addArgument(ptcNormalsCL, 8);
    cout << "Arguments for computing excident added" << endl;

    prepareBuffers->addArgument(excident, 0);
    prepareBuffers->addArgument(incident, 1);
    prepareBuffers->addArgument(indirect, 2);
    prepareBuffers->addArgument(ptcClrCL, 3);
    cout << "Arguments for preparing buffers added" << endl;
}

void SaveFormFactorsStatistic(const string& output) {
    map<uint, uint> lengths;
    ofstream out(output);

    uint maxLength = 0;
    uint sumLength = 0;

    for (auto& row: ff) {
        lengths[row.size()]++;
        maxLength = max(maxLength, row.size());
        sumLength += row.size();
    }
    for (auto& len: lengths) {
        out << len.first << ' ' << len.second << endl;
    }

    out << endl << "******************" << endl << endl;

    out << sumLength << ' ' << maxLength * ff.size() << endl;

    out.close();
}

int main(int argc, char **argv) {
    cout << "Start" << endl;
    InitializeGLUT(argc, argv);
    cout << "GLUT inited" << endl;
	glewInit();
	cout << "glew inited" << endl;
	clewInit(L"OpenCL.dll");
	cout << "clew inited" << endl;
    ReadSplitedData("Precompute/data/colored-sponza/Model10.bin");
    cout << "Data readed" << endl;
    ReadMaterials("Scenes\\colored-sponza\\sponza_exported\\hydra_profile_generated.xml");
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
    ReadPatches("Precompute/data/colored-sponza/Patches10.bin");
    cout << "Patches read: " << ptcColors.size() << endl;
    ReadFormFactors("Precompute/data/colored-sponza/FF10.bin");
    cout << "Form-factors read" << endl;
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
