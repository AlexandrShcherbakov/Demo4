#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Utility.h"

using namespace std;

string sceneName = "colored-sponza";

const string GenPath(const string& name, const int size, const bool image=false) {
    stringstream ss;
    string suffix = image ? ".dds" : ".bin";
    ss << "../Scenes/" << sceneName << "/" << name << size << suffix;
    return ss.str();
}

const vector<VM::vec3> ReadFF(const int size) {
    ifstream in(GenPath("FF", size), ios::binary | ios::in);
    int ffSize;
    in.read((char*)&ffSize, sizeof(ffSize));
    vector<VM::vec3> data(ffSize * ffSize);
    for (int i = 0; i < ffSize; ++i) {
        for (int j = 0; j < ffSize; ++j) {
            int index = i * ffSize + j;
            in.read((char*)&data[index], sizeof(data[index]));
        }
    }
    return data;
}

typedef unsigned char byte;

const vector<byte> PrepareColors(const vector<VM::vec3>& data, const int size) {
    const int components = VM::vec3::Components;
    vector<byte> colors(data.size() * components);
    for (uint i = 0; i < data.size(); ++i) {
        VM::vec3 color = data[i];// * 255;
        for (int j = 0; j < components; ++j) {
            colors[i * components + j] = round(color[j]);
        }
    }
    return colors;
}

void SaveImage(const vector<byte>& data, const int size) {
    const int channels = VM::vec3::Components;
    const int side = static_cast<int>(sqrt(data.size() / channels));
    SOIL_save_image(GenPath("FF", size, true).c_str(), SOIL_SAVE_TYPE_DDS, side, side, channels, data.data());
}

void InitializeGLUT(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitContextVersion(4, 3);
}

int main(int argc, char** argv) {
    InitializeGLUT(argc, argv);
    int size = 20;
    if (argc > 1) {
        istringstream iss(argv[1]);
        iss >> size;
        cout << "Current size: " << size << endl;
    }
    SaveImage(PrepareColors(ReadFF(size), size), size);
}
