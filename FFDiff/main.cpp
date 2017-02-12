#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "Utility.h"

using namespace std;

void InitializeGLUT(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitContextVersion(4, 3);
}

const string GenPath(const string& name, const int size, const bool image=false) {
    stringstream ss;
    string suffix = image ? ".dds" : ".bin";
    ss << "../Scenes/colored-sponza/" << name << size << suffix;
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

const vector<VM::vec3> ReadTexture(const int size) {
    int width, height, channels;
    auto rawData = SOIL_load_image(GenPath("FF", size, true).c_str(), &width, &height, &channels, 0);

    VM::vec3 minValue, maxValue;
    ifstream in(GenPath("AdditionalInfo", size), ios::binary | ios::in);
    in.read((char*)&maxValue, sizeof(maxValue));

    vector<VM::vec3> ff;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            VM::vec3 value;
            for (int h = 0; h < channels; ++h) {
                value[h] = rawData[(i * width + j) * channels + h];
                //value[h] = exp(value[h] / 255 * 25 - 25) - 1e-9f;
            }
            ff.push_back(value);
        }
    }
    return ff;
}

void Compare(const vector<VM::vec3>& ff1, const vector<VM::vec3>& ff2) {
    cout << ff1.size() << ' ' << ff2.size() << endl;
    float result = 0;
    for (uint i = 0; i < ff1.size(); ++i) {
        result += length(ff1[i] - ff2[i]);
    }
    cout << result << endl;
}

int main(int argc, char** argv) {
    InitializeGLUT(argc, argv);
    int size = 20;
    if (argc > 1) {
        istringstream iss(argv[1]);
        iss >> size;
        cout << "Current size: " << size << endl;
    }
    Compare(ReadFF(size), ReadTexture(size));
}
