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
