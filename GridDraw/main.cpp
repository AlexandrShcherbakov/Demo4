#include <fstream>
#include <iostream>
#include <vector>

#include "HydraExport.h"
#include "Utility.h"

using namespace std;

HydraGeomData hyFile;
vector<VM::vec4> points;
vector<uint> indices;

GL::Mesh *mesh;
GL::Camera camera;

void RenderLayouts() {
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mesh->DrawWithIndices(GL_LINES);
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
	glutCreateWindow("Draw Grid");
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
    ifstream in(path, ios::in | ios::binary);
    uint size;
    in.read((char*)&size, sizeof(size));
	for (uint i = 0; i < size; i++) {
        VM::vec4 point;
        VM::vec4 normal;
        VM::vec2 texCoord;
        uint materialNumber;
        VM::i16vec4 relIndices;
        VM::vec4 weights;

        in.read((char*)&point, sizeof(point));
        in.read((char*)&normal, sizeof(normal));
        in.read((char*)&texCoord, sizeof(texCoord));
        in.read((char*)&materialNumber, sizeof(materialNumber));
        in.read((char*)&relIndices, sizeof(relIndices));
        in.read((char*)&weights, sizeof(weights));
        points.push_back(point);
	}
    uint indicesSize;
    in.read((char*)&indicesSize, sizeof(indicesSize));
    indices.resize(indicesSize * 2);
    for (uint i = 0; i < indicesSize / 3; ++i) {
        uint triangle[3];
        in.read((char*)&triangle[0], sizeof(triangle[0]));
        in.read((char*)&triangle[1], sizeof(triangle[1]));
        in.read((char*)&triangle[2], sizeof(triangle[2]));
        indices.push_back(triangle[0]);
        indices.push_back(triangle[1]);
        indices.push_back(triangle[1]);
        indices.push_back(triangle[2]);
        indices.push_back(triangle[2]);
        indices.push_back(triangle[0]);
    }
	in.close();
}

void ReadOldData(const string &path) {
    hyFile.read(path);
    for (uint i = 0; i < hyFile.getVerticesNumber(); i++) {
        points.push_back(VM::vec4(hyFile.getVertexPositionsFloat4Array() + 4 * i));
    }
    for (uint i = 0; i < points.size() / 3; ++i) {
        indices.push_back(3 * i + 0);
        indices.push_back(3 * i + 1);
        indices.push_back(3 * i + 1);
        indices.push_back(3 * i + 2);
        indices.push_back(3 * i + 2);
        indices.push_back(3 * i + 0);
    }
}

void CreateBuffers(GL::Vec4ArrayBuffer& pointsBuffer, GL::IndexBuffer& indicesBuffer) {
    pointsBuffer.SetData(points);
    indicesBuffer.SetData(indices);
}

void CreateMeshes() {
    mesh = new GL::Mesh();
}

GL::ShaderProgram* ReadShader() {
    auto prog = new GL::ShaderProgram();
    prog->LoadFromFile("grid");
    return prog;
}

void AddBuffersToMeshes(
    GL::Vec4ArrayBuffer& pointsBuffer,
    GL::IndexBuffer& indicesBuffer
) {
    mesh->bindBuffer(pointsBuffer, "points");
    mesh->bindIndicesBuffer(indicesBuffer);
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

void AddCameraToMeshes() {
	mesh->setCamera(&camera);
}

void AddShaderProgramToMeshes(GL::ShaderProgram& shader) {
    mesh->setShaderProgram(shader);
}

int main(int argc, char **argv) {
    cout << "Start" << endl;
    InitializeGLUT(argc, argv);
    cout << "GLUT inited" << endl;
	glewInit();
	cout << "glew inited" << endl;
    ReadData("../Scenes/colored-sponza/Model37.bin");
    //ReadOldData("../Scenes/colored-sponza/sponza_exported/scene.vsgf");
    cout << "Data readed" << endl;
    GL::Vec4ArrayBuffer pointsBuffer;
    GL::IndexBuffer indicesBuffer;
    CreateBuffers(pointsBuffer, indicesBuffer);
    cout << "Buffers created" << endl;
    CreateMeshes();
    cout << "Meshes created" << endl;
    GL::ShaderProgram *shader = ReadShader();
    cout << "Shaders readed" << endl;
    AddBuffersToMeshes(pointsBuffer, indicesBuffer, *shader);
    cout << "Buffers added" << endl;
    CreateCamera();
    cout << "Camera created" << endl;
    AddCameraToMeshes();
    cout << "Camera added to meshes" << endl;
    AddShaderProgramToMeshes(*shader);
    cout << "Shader programs added to meshes" << endl;
    glutMainLoop();
    return 0;
}
