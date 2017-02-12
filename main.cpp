#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "Utility.h"
#include "HydraExport.h"

//#define TIMESTAMPS
//#define LABORATORY

using namespace std;

HydraGeomData hyFile;
vector<VM::vec4> points, normals;
vector<VM::vec2> texCoords;
vector<uint> materialNum, indices;
vector<VM::vec4> relationWeights;
vector<VM::uvec4> relationIndices;

vector<VM::vec4> ptcPoints;
vector<VM::vec4> ptcNormals;

map<uint, vector<uint> > splitedIndices;

GL::Framebuffer *shadowMapScreen;

map<uint, GL::Mesh> meshes;

GL::Mesh* fullGeometry;
GL::Texture* shadowMap;
GL::Vec4ArrayBuffer* indirectBuffer;

GL::DirectionalLightSource light;

GL::Camera camera;

GL::ComputeShader *radiosity, *computeIndirect, *computeEmission, *prepareBuffers;

GL::Vec4StorageBuffer *ffValues;
GL::Uvec4StorageBuffer *indirectRelIndices;
GL::Vec2StorageBuffer *rand_coords;
GL::Vec4StorageBuffer *excident, *ptcPointsBuf, *ptcNormalsBuf, *incident, *indirect, *indirectRelWeights, *pointsIncident;
GL::FloatStorageBuffer *correctValues;
GL::UintStorageBuffer *correctIndices;

bool CreateFF = true;
bool StartLightMove = false;

int radiosityIterations = 1;

void FinishProgram();

string sceneName = "colored-sponza";
uint voxelConst = 20;
GLuint ffTexture;

void UpdateUniforms() {
    computeEmission->SetUniform("lightMatrix", light.getMatrix());
    computeEmission->SetUniform("lightPosition", light.position);
    computeEmission->SetUniform("lightDirection", light.direction);
    computeEmission->SetUniform("innerRadius", light.GetInnerRadius());
    computeEmission->SetUniform("outterRadius", light.GetOutterRadius());
    computeEmission->SetUniform("shadowMap", 0);
    computeEmission->Bind();
    glActiveTexture(GL_TEXTURE0); GL::CHECK_GL_ERRORS;
    shadowMap->Bind();
    computeEmission->Unbind();
}

void BindComputeEmission() {
    computeEmission->Bind();
    ptcPointsBuf->BindBase(0);
    rand_coords->BindBase(1);
    ptcNormalsBuf->BindBase(3);
    excident->BindBase(4);
    indirect->BindBase(5);
}

void BindPrepareBuffers() {
    prepareBuffers->Bind();
    excident->BindBase(4);
    indirect->BindBase(5);
    incident->BindBase(6);
}

void BindRadiosity() {
    radiosity->Bind();
    excident->BindBase(4);
    incident->BindBase(6);
    correctIndices->BindBase(0);
    correctValues->BindBase(1);
    //ffValues->BindBase(7);
}

void BindComputeIndirect() {
    computeIndirect->Bind();
    indirectRelIndices->BindBase(0);
    indirectRelWeights->BindBase(1);
    indirect->BindBase(5);
    pointsIncident->BindBase(3);
}

void CountRadiosity(ofstream& logger) {
    //UpdateCLBuffers();
#ifdef TIMESTAMPS
    clock_t timestamp = clock();
#endif // TIMESTAMPS
    BindComputeEmission();
    computeEmission->Run(ptcNormals.size() / 256, 1, 1);
#ifdef TIMESTAMPS
    logger << "Compute emission " << clock() - timestamp << endl;
#endif // TIMESTAMPS
    for (int i = 0; i < radiosityIterations; ++i) {
#ifdef TIMESTAMPS
        timestamp = clock();
#endif // TIMESTAMPS
        BindRadiosity();
        radiosity->Run(ptcNormals.size() / 256, 1, 1);
#ifdef TIMESTAMPS
        logger << "Radiosity " << clock() - timestamp << endl;
        timestamp = clock();
#endif // TIMESTAMPS
        BindPrepareBuffers();
        prepareBuffers->Run(ptcNormals.size() / 256, 1, 1);

#ifdef TIMESTAMPS
        logger << "Buffers preparing " << clock() - timestamp << endl;
#endif // TIMESTAMPS
    }

#ifdef TIMESTAMPS
    timestamp = clock();
#endif // TIMESTAMPS
    BindComputeIndirect();
    computeIndirect->Run(points.size() / 256, 1, 1);
#ifdef TIMESTAMPS
    logger << "Indirect computation " << clock() - timestamp << endl;
#endif // TIMESTAMPS
}

void RenderLayouts() {
#ifdef TIMESTAMPS
    stringstream ss;
    ss << "../logs/" << sceneName << " " << ptcNormals.size() << " (" << voxelConst << ").txt";
    static ofstream logger(ss.str());
    logger << "START_FRAME" << endl;
    clock_t timestamp = clock();
#else
    static ofstream logger;
#endif // TIMESTAMPS

#ifdef TIMESTAMPS
    logger << "Update CL Buffers " << clock() - timestamp << endl;
    timestamp = clock();
#endif // TIMESTAMPS
    //Render shadow
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fullGeometry->DrawWithIndices(GL_TRIANGLES, shadowMapScreen);
#ifdef TIMESTAMPS
	logger << "Render shadowmap " << clock() - timestamp << endl;
	timestamp = clock();
#endif // TIMESTAMPS
	//Count radiosity
	UpdateUniforms();
	CountRadiosity(logger);

    std::vector<VM::vec4> indir = pointsIncident->GetData();
    indirectBuffer->SetData(indir);
#ifdef TIMESTAMPS
    logger << "Full radiosity computing " << clock() - timestamp << endl;
    timestamp = clock();
#endif // TIMESTAMPS
	//Render scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	for (auto &it: meshes) {
        it.second.DrawWithIndices();
	}
#ifdef TIMESTAMPS
    logger << "Render scene " << clock() - timestamp << endl;
    logger << "END_FRAME" << endl;
#endif // TIMESTAMPS
	glutSwapBuffers();

    if (StartLightMove) {
        light.direction = VM::normalize(light.direction + VM::vec3(0, 0, -0.005));
    }

#ifdef TIMESTAMPS
    static int framesCnt = 0;
    if (1000 < framesCnt++) {
        FinishProgram();
    }
#endif // TIMESTAMPS
}

void FreeResources() {
    radiosity = nullptr;
    computeIndirect = nullptr;
    computeEmission = nullptr;
    prepareBuffers = nullptr;

    rand_coords = nullptr;
    excident = nullptr;
    incident = nullptr;
    indirectRelIndices = nullptr;
    indirectRelWeights = nullptr;
    pointsIncident = nullptr;
    indirect = nullptr;
}

void FinishProgram() {
    FreeResources();
    glutDestroyWindow(glutGetWindow());
    exit(0);
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
        camera.rotateY(0.05);
	} else if (key == GLUT_KEY_LEFT) {
	    camera.rotateY(-0.05);
	} else if (key == GLUT_KEY_UP) {
        camera.rotateTop(-0.05);
	} else if (key == GLUT_KEY_DOWN) {
		camera.rotateTop(0.05);
	}
}

void IdleFunc() {
    glutPostRedisplay();
}

void MouseMove(int x, int y) {
    const static float centerX = 400.0f, centerY = 300.0f;
    if (x != 400 || y != 300) {
		//camera.rotateY((x - centerX) / 1000.0f);
		//camera.rotateTop((y - centerY) / 1000.0f);
		//glutWarpPointer(400, 300);
    }
}

void MouseClick(int button, int state, int x, int y) {
}

void InitializeGLUT(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitContextVersion(3, 0);
	glutInitWindowPosition(40, 40);
	glutInitWindowSize(1024, 768);
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

string GenScenePath(const string& partName, const string& suffix=".bin") {
    stringstream path;
    path << "../Scenes/" << sceneName << "/" << partName << voxelConst << suffix;
    return path.str();
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

void ReadSplitedData() {
    ifstream in(GenScenePath("Model"), ios::in | ios::binary);
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

void ReadPatches() {
    ifstream in(GenScenePath("Patches"), ios::in | ios::binary);
    uint size;
    in.read((char*)&size, sizeof(size));
    ptcPoints.resize(size * 4);
    ptcNormals.resize(size);
    for (uint i = 0; i < size; ++i) {
        in.read((char*)&ptcNormals[i], sizeof(ptcNormals[i]));
        for (uint j = 0; j < 4; ++j) {
            in.read((char*)&ptcPoints[4 * i + j], sizeof(ptcPoints[4 * i + j]));
        }
    }
    in.close();
}

void ReadFormFactors(vector<VM::vec4>& ffValues) {
    ifstream in(GenScenePath("FF"), ios::in | ios::binary);
    uint size;
    in.read((char*)&size, sizeof(size));
    ffValues.resize(size * size);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            VM::vec3 value;
            in.read((char*)&value, sizeof(value));
            ffValues[i * size + j] = VM::vec4(value, 1);
        }
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
			tex = new GL::Texture();
			tex->SetSlot(1);
            tex->LoadFromFile(s);
        }
        if (splitedIndices.count(ind)) {
            if (tex != nullptr) {
                meshes[ind].AddTexture("material_texture", tex);
            }
            meshes[ind].SetAmbientColor(color);
        }
    }
}

GL::Texture InitShadowMap() {
    shadowMapScreen = new GL::Framebuffer(1024, 768);
    shadowMap = new GL::Texture(2048, 2048);
    shadowMap->SetSlot(0);
    shadowMapScreen->AttachTexture(*shadowMap);
	return *shadowMap;
}

void SplitIndicesByMaterial() {
    for (uint i = 0; i < indices.size(); ++i) {
        splitedIndices[materialNum[indices[i]]].push_back(indices[i]);
    }
}

void CreateBuffers(
    std::map<uint, GL::IndexBuffer>& indicesBuffers,
    GL::Vec4ArrayBuffer& pointsBuffer,
    GL::Vec4ArrayBuffer& normalsBuffer,
    GL::Vec2ArrayBuffer& texCoordsBuffer,
    GL::Vec4ArrayBuffer& indirectBuffer,
    GL::IndexBuffer& fullIndices
) {
    for (auto &it: splitedIndices) {
        indicesBuffers[it.first].SetData(it.second);
    }
    pointsBuffer.SetData(points);
    normalsBuffer.SetData(normals);
    texCoordsBuffer.SetData(texCoords);
    indirectBuffer.SetData(normals);
    fullIndices.SetData(indices);
}

void CreateMeshes(std::map<uint, GL::IndexBuffer> indicesBuffers) {
    for (auto &it: indicesBuffers) {
        meshes[it.first] = GL::Mesh();
    }
    fullGeometry = new GL::Mesh();
}

void ReadShaders(
    GL::ShaderProgram& texturedShader,
    GL::ShaderProgram& coloredShader,
    GL::ShaderProgram& shadowMapShader
) {
    texturedShader.LoadFromFile("textured");
    coloredShader.LoadFromFile("colored");
    shadowMapShader.LoadFromFile("ShadowMap");
}

void AddBuffersToMeshes(
    std::map<uint, GL::IndexBuffer>& indicesBuffers,
    GL::Vec4ArrayBuffer& pointsBuffer,
    GL::Vec4ArrayBuffer& normalsBuffer,
    GL::Vec2ArrayBuffer& texCoordsBuffer,
    GL::Vec4ArrayBuffer& indirectBuffer,
    GL::IndexBuffer& fullIndices,
    GL::ShaderProgram& texturedShader,
    GL::ShaderProgram& coloredShader,
    GL::ShaderProgram& shadowMapShader
) {
    for (auto &it: meshes) {
        if (it.second.HasTextures()) {
            it.second.BindBuffer(texCoordsBuffer, "texCoord");
        }
        it.second.BindBuffer(pointsBuffer, "points");
        it.second.BindBuffer(normalsBuffer, "normal");
        it.second.BindBuffer(indirectBuffer, "indirect");
        it.second.BindIndicesBuffer(indicesBuffers[it.first]);
    }
    fullGeometry->BindBuffer(pointsBuffer, "points");
    fullGeometry->BindIndicesBuffer(fullIndices);
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
    camera.screenRatio = 1024.0 / 768.0;
    camera.up = VM::vec3(0, 1, 0);
    camera.zfar = 10000.0f;
    camera.znear = 0.001f;
}

void AddLightToMeshes() {
    for (auto &it: meshes) {
        it.second.AddLight("light", light);
    }
}

void AddCameraToMeshes() {
    for (auto &it: meshes) {
        it.second.SetCamera(camera);
    }
    fullGeometry->SetCamera(light);
}

void AddShadowMapToMeshes(GL::Texture& shadowMap) {
    for (auto &it: meshes) {
        it.second.AddTexture("shadowMap", &shadowMap);
    }
}

void AddShaderProgramToMeshes(
    GL::ShaderProgram& texturedShader,
    GL::ShaderProgram& coloredShader,
    GL::ShaderProgram& shadowMapShader
) {
    for (auto &it: meshes) {
        if (!it.second.HasTextures())
            it.second.SetShaderProgram(coloredShader);
		else
			it.second.SetShaderProgram(texturedShader);
    }
    fullGeometry->SetShaderProgram(shadowMapShader);
}


void CreateComputeShaders() {
    radiosity = new GL::ComputeShader();
    radiosity->LoadFromFile("Radiosity");

    computeEmission = new GL::ComputeShader();
    computeEmission->LoadFromFile("ComputeEmission");

    prepareBuffers = new GL::ComputeShader();
    prepareBuffers->LoadFromFile("PrepareBuffers");
}

void CreateComputeBuffers(const GL::Texture& shadowMap) {
    vector<VM::vec4> zeros(ptcNormals.size(), VM::vec4(0.0f));
    rand_coords = new GL::Vec2StorageBuffer();
    ptcPointsBuf = new GL::Vec4StorageBuffer();
    ptcNormalsBuf = new GL::Vec4StorageBuffer();
    excident = new GL::Vec4StorageBuffer();
    excident->SetData(zeros);
    incident = new GL::Vec4StorageBuffer();
    incident->SetData(zeros);
    indirect = new GL::Vec4StorageBuffer();
    indirect->SetData(zeros);
}

void PrepareRadiosityKernel() {
    vector<VM::vec4> ffValuesVec;
    ReadFormFactors(ffValuesVec);
    cout << ffValuesVec.size() << endl;

    ffValues = new GL::Vec4StorageBuffer();
    cout << "FF textures created" << endl;

    //ffValues->SetData(ffValuesVec);
    ffValuesVec.clear();
    cout << "FF data set" << endl;

    ifstream in(GenScenePath("Corrector"), ios::in | ios::binary);
    int correctLimit;
    in.read((char*)&correctLimit, sizeof(correctLimit));

    vector<float> correctValuesVec;
    vector<uint> correctIndicesVec;
    for (uint i = 0; i < correctLimit * 3 * ptcNormals.size(); ++i) {
        float value;
        uint index;
        in.read((char*)&value, sizeof(value));
        in.read((char*)&index, sizeof(index));
        correctValuesVec.push_back(value);
        correctIndicesVec.push_back(index);
    }
    cout << correctIndicesVec[0] << ' ' << correctValuesVec[0] << endl;

    correctValues = new GL::FloatStorageBuffer();
    correctIndices = new GL::UintStorageBuffer();

    correctValues->SetData(correctValuesVec);
    correctIndices->SetData(correctIndicesVec);

    radiosity->SetUniform("correctLimit", correctLimit);

    ffTexture = SOIL_load_OGL_texture(GenScenePath("FF", ".dds").c_str(), 3, 0, SOIL_FLAG_DDS_LOAD_DIRECT);

    radiosity->Bind();
    glActiveTexture(GL_TEXTURE2); GL::CHECK_GL_ERRORS;
    glBindTexture(GL_TEXTURE_2D, ffTexture); GL::CHECK_GL_ERRORS;
    radiosity->SetUniform("ffTexture", 2);
    excident->BindBase(0);
    ffValues->BindBase(1);
    incident->BindBase(2);
    radiosity->Unbind();
    cout << "Radiosity kernel prepared" << endl;
}

void PrepareComputeIndirectKernel(
    vector<VM::uvec4>& relationIndices,
    vector<VM::vec4>& relationWeights
) {
    indirectRelIndices = new GL::Uvec4StorageBuffer();
    indirectRelWeights = new GL::Vec4StorageBuffer();
    pointsIncident = new GL::Vec4StorageBuffer();
    cout << "Textures created" << endl;

    indirectRelIndices->SetData(relationIndices);
    indirectRelWeights->SetData(relationWeights);
    pointsIncident->SetData(nullptr, points.size() * sizeof(VM::vec4));
    cout << "Data set" << endl;

    computeIndirect = new GL::ComputeShader();
    computeIndirect->LoadFromFile("ComputeIndirect");
    cout << "Shader created" << endl;

    computeIndirect->Bind();
    indirectRelIndices->BindBase(0);
    indirectRelWeights->BindBase(1);
    indirect->BindBase(2);
    pointsIncident->BindBase(3);
    computeIndirect->Unbind();
    cout << "Arguments set" << endl;
}

void FillCLBuffers() {
    vector<VM::vec2> coords(20);
    for (uint i = 0; i < coords.size(); ++i) {
        coords[i].x = static_cast<float>(rand()) / RAND_MAX;
        coords[i].y = static_cast<float>(rand()) / RAND_MAX;
        coords[i] = VM::normalize(coords[i]);
    }
    rand_coords->SetData(coords);
    cout << "Random coords loaded" << endl;

    ptcPointsBuf->SetData(ptcPoints);
    cout << "Patches points loaded" << endl;

    ptcNormalsBuf->SetData(ptcNormals);
    cout << "Patches normals loaded" << endl;

    PrepareRadiosityKernel();
    PrepareComputeIndirectKernel(relationIndices, relationWeights);
}

void SetArgumentsForKernels() {
    computeEmission->Bind();
    ptcPointsBuf->BindBase(1);
    rand_coords->BindBase(2);
    ptcNormalsBuf->BindBase(4);
    excident->BindBase(5);
    indirect->BindBase(6);
    computeEmission->Unbind();
    cout << "Arguments for computing excident added" << endl;

    prepareBuffers->Bind();
    excident->BindBase(0);
    incident->BindBase(1);
    indirect->BindBase(2);
    prepareBuffers->Unbind();
    cout << "Arguments for preparing buffers added" << endl;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        istringstream iss(argv[1]);
        iss >> voxelConst;
        cout << "Size set " << voxelConst << endl;
    }

    cout << "Start" << endl;
    InitializeGLUT(argc, argv);
    cout << "GLUT inited" << endl;
	glewInit();
	cout << "glew inited" << endl;
    ReadSplitedData();
    cout << "Data readed" << endl;
    cout << "Buffers created" << endl;
    GL::ShaderProgram texturedShader, coloredShader;
    GL::ShaderProgram shadowMapShader;
    ReadShaders(texturedShader, coloredShader, shadowMapShader);
    cout << "Shaders readed" << endl;
    SplitIndicesByMaterial();
    map<uint, GL::IndexBuffer> indicesBuffers;
    GL::Vec4ArrayBuffer pointsBuffer, normalsBuffer;
    indirectBuffer = new GL::Vec4ArrayBuffer();
    GL::Vec2ArrayBuffer texCoordsBuffer;
    GL::IndexBuffer fullIndices;
    CreateBuffers(indicesBuffers, pointsBuffer, normalsBuffer, texCoordsBuffer, *indirectBuffer, fullIndices);
    cout << "Indices splited" << endl;
    CreateMeshes(indicesBuffers);
    cout << "Meshes created" << endl;
    map<uint, GL::Material> materials;
    ReadMaterials("..\\Scenes\\colored-sponza\\sponza_exported\\hydra_profile_generated.xml");
    cout << "Materials readed" << endl;
    GL::Texture shadowMap = InitShadowMap();
    cout << "ShadowMap inited" << endl;
    AddShaderProgramToMeshes(texturedShader, coloredShader, shadowMapShader);
    cout << "Shader programs added to meshes" << endl;
    AddBuffersToMeshes(
        indicesBuffers, pointsBuffer,
        normalsBuffer, texCoordsBuffer,
        *indirectBuffer, fullIndices,
        texturedShader, coloredShader,
        shadowMapShader
    );
    cout << "Buffers added" << endl;
    CreateLight();
    cout << "Light source created" << endl;
    CreateCamera();
    cout << "Camera created" << endl;
    AddLightToMeshes();
    cout << "Lights added to meshes" << endl;
    AddCameraToMeshes();
    cout << "Camera added to meshes" << endl;
    AddShadowMapToMeshes(shadowMap);
    cout << "ShadowMap added to meshes" << endl;
    ReadPatches();
    cout << "Patches read: " << ptcNormals.size() << endl;
    CreateComputeShaders();
    cout << "CL kernels created" << endl;
    CreateComputeBuffers(shadowMap);
    cout << "CL buffers created" << endl;
    FillCLBuffers();
    cout << "Fill CL buffers" << endl;
    SetArgumentsForKernels();
    cout << "Arguments added" << endl;

    glutMainLoop();
    return 0;
}
