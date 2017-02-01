#include <iostream>
#include <fstream>

#include "Utility.h"
#include "GeometryLib\Octree.h"

using namespace std;

vector<VM::vec4> points, colors;

GL::Mesh *mesh;

GL::SpotLightSource light;

GL::Camera camera;


void RenderLayouts() {
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mesh->Draw(points.size());
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
	glutCreateWindow("Draw Voxels");
	glutWarpPointer(400, 300);
	glutSetCursor(GLUT_CURSOR_NONE);

	glutDisplayFunc(RenderLayouts);
    glutKeyboardFunc(KeyboardEvents);
    glutSpecialFunc(SpecialButtons);
    glutIdleFunc(IdleFunc);
    //glutPassiveMotionFunc(MouseMove);
    glutMouseFunc(MouseClick);
}

void ReadData(const string &path, const string &colorsInput="") {
    ifstream in(path, ios::in | ios::binary);
    ifstream colIn(colorsInput, ios::in | ios::binary);
    uint size;
    in.read((char*)&size, sizeof(size));
	for (uint i = 0; i < size; i++) {
        VM::vec4 point;
        VM::vec4 color;
        VM::vec4 normal;
        in.read((char*)&color, sizeof(color));
        if (!colorsInput.empty()) {
            colIn.read((char*)&color, sizeof(color));
        }
        in.read((char*)&normal, sizeof(normal));
        VM::vec4 pnts[4];
        for (uint j = 0; j < 4; ++j) {
            in.read((char*)&pnts[j], sizeof(pnts[j]));
        }
        for (uint j = 2; j < 4; ++j) {
            points.push_back(pnts[0]);
            points.push_back(pnts[j - 1]);
            points.push_back(pnts[j]);
            colors.push_back(color);
            colors.push_back(color);
            colors.push_back(color);
        }
	}
	in.close();
    colIn.close();
}


void CreateBuffers(GL::Vec4ArrayBuffer& pointsBuffer, GL::Vec4ArrayBuffer& colorsBuffer) {
    pointsBuffer.SetData(points);
    colorsBuffer.SetData(colors);
}

void CreateMeshes() {
    mesh = new GL::Mesh();
}

GL::ShaderProgram* ReadShader() {
    GL::ShaderProgram * program = new GL::ShaderProgram();
    program->LoadFromFile("colored");
    return program;
}

void AddBuffersToMeshes(
    GL::Vec4ArrayBuffer& pointsBuffer,
    GL::Vec4ArrayBuffer& colorsBuffer,
    GL::ShaderProgram& shader
) {
    mesh->BindBuffer(pointsBuffer, "points");
    mesh->BindBuffer(colorsBuffer, "colors");
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

void AddCameraToShaders() {

}

void AddCameraToMeshes() {
	mesh->SetCamera(camera);
}

void AddShaderProgramToMeshes(GL::ShaderProgram& shader) {
    mesh->SetShaderProgram(shader);
}

void ReadFFForColors(const string& input, const uint row) {
    ifstream in(input, ios::binary | ios::in);
    uint globalSize;
    in.read((char*)&globalSize, sizeof(globalSize));
    for (uint i = 0; i < row; ++i) {
        uint size;
        in.read((char*)&size, sizeof(size));
        uint idx;
        float value;
        for (uint j = 0; j < size; ++j) {
            in.read((char*)&idx, sizeof(idx));
            in.read((char*)&value, sizeof(value));
        }
    }
    for (uint i = 0; i < colors.size(); ++i) {
        colors[i] = VM::vec4(0, 0, 0, 0);
    }
    uint size;
    in.read((char*)&size, sizeof(size));
    uint idx;
    float value;
    uint maxInd;
    float maxValue = 0;
    for (uint j = 0; j < size; ++j) {
        in.read((char*)&idx, sizeof(idx));
        in.read((char*)&value, sizeof(value));
        for (uint i = 0; i < 6; ++i) {
            colors[6 * idx + i] = VM::vec4(value, value, value, 1);
        }
        if (value > maxValue) {
            maxValue = value;
            maxInd = idx;
        }
    }
    cout << maxInd << ' ' << maxValue << endl;
    for (uint i = 0; i < 6; ++i) {
        colors[6 * row + i] = VM::vec4(1, 0, 0, 1);
    }
    in.close();
}

int main(int argc, char **argv) {
    cout << "Start" << endl;
    InitializeGLUT(argc, argv);
    cout << "GLUT inited" << endl;
	glewInit();
	cout << "glew inited" << endl;
    ReadData("../../Scenes/colored-sponza/Patches37.bin", "../../lightning/emission37.bin");
    //ReadFFForColors("../Precompute/data/ff20.bin", 512);
    cout << "Data readed" << endl;
    GL::Vec4ArrayBuffer pointsBuffer, colorsBuffer;
    CreateBuffers(pointsBuffer, colorsBuffer);
    cout << "Buffers created" << endl;
    CreateMeshes();
    cout << "Meshes created" << endl;
    GL::ShaderProgram *shader = ReadShader();
    cout << "Shaders readed" << endl;
    AddShaderProgramToMeshes(*shader);
    cout << "Shader programs added to meshes" << endl;
    AddBuffersToMeshes(pointsBuffer, colorsBuffer, *shader);
    cout << "Buffers added" << endl;
    CreateCamera();
    cout << "Camera created" << endl;
    AddCameraToShaders();
    cout << "Camera added to shaders" << endl;
    AddCameraToMeshes();
    cout << "Camera added to meshes" << endl;
    glutMainLoop();
    return 0;
}
