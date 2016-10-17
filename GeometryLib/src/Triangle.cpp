#include "Triangle.h"

using namespace VM;
using namespace std;

Triangle::Triangle() {
    ImagePointer = nullptr;
    AmbientColor = vec4(0, 0, 0, 0);
    Points.fill(vec4(0, 0, 0, 0));
    Normals.fill(vec4(0, 0, 0, 0));
    TexCoords.fill(vec2(0, 0));
}

void Triangle::InheritParametersFrom(const Triangle& parent) {
    this->AmbientColor = parent.AmbientColor;
    this->ImagePointer = parent.ImagePointer;
    this->MaterialNumber = parent.MaterialNumber;
}

void Triangle::SetPoints(const vec4* points, const vec4* normals, const vec2* texCoords) {
    for (uint i = 0; i < 3; ++i) {
		Points[i] = points[i];
		Normals[i] = normals[i];
		TexCoords[i] = texCoords[i];
    }
}

vec4 Triangle::MeanNormal() const {
    vec4 result(0, 0, 0, 0);
    for (auto &normal: this->Normals) {
        result += normal;
    }
	return result / Normals.size();
}
