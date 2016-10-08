#include "CubeWithTriangles.h"

using namespace std;
using namespace VM;

CubeWithTriangles::CubeWithTriangles(const VM::vec4& minPoint, const VM::vec4& maxPoint) {
    this->MaxPoint = maxPoint;
    this->MinPoint = minPoint;
}

vector<vec4> CubeWithTriangles::GetPoints() const {
    vector<vec4> points;
    for (auto& triangle: Triangles) {
        points.insert(points.end(), triangle.PointsBegin(), triangle.PointsEnd());
    }
    return points;
}

vector<vec4> CubeWithTriangles::GetNormals() const {
	vector<vec4> normals;
    for (auto& triangle: Triangles) {
        normals.insert(normals.end(), triangle.NormalsBegin(), triangle.NormalsEnd());
    }
    return normals;
}

vector<vec2> CubeWithTriangles::GetTexCoords() const {
	vector<vec2> texCoords;
    for (auto& triangle: Triangles) {
        texCoords.insert(texCoords.end(), triangle.TexCoordsBegin(), triangle.TexCoordsEnd());
    }
    return texCoords;
}

vector<uint> CubeWithTriangles::GetMaterialNumbers() const {
    vector<uint> materialNumbers;
    for (auto& triangle: Triangles) {
        for (uint i = 0; i < 3; ++i) {
            materialNumbers.push_back(triangle.GetMaterialNumber());
        }
    }
    return materialNumbers;
}

vector<vec4> CubeWithTriangles::GetAmbientColors() const {
	vector<vec4> ambientColors;
    for (auto& triangle: Triangles) {
        for (uint i = 0; i < 3; ++i) {
            ambientColors.push_back(triangle.GetAmbientColor());
        }
    }
    return ambientColors;
}

void CubeWithTriangles::SetIndices(uint& index) {
    for (uint i = 0; i < Triangles.size(); ++i)
		Indices[i] = index++;
}
