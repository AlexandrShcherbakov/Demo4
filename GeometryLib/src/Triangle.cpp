#include "Triangle.h"

using namespace VM;
using namespace std;

Triangle::Triangle() {
    ImagePointer = nullptr;
    AmbientColor = vec4(0, 0, 0, 0);
}

void Triangle::SetPoints(const vec4* points, const vec4* normals, const vec2* texCoords) {
    for (uint i = 0; i < 3; ++i) {
		Points[i] = points[i];
		Normals[i] = normals[i];
		TexCoords[i] = texCoords[i];
    }
}

class PolygonForTriangles {
public:
    PolygonForTriangles(const Triangle& triangle) {
        Points = vector<vec4>(triangle.PointsBegin(), triangle.PointsEnd());
        Normals = vector<vec4>(triangle.NormalsBegin(), triangle.NormalsEnd());
        TexCoords = vector<vec2>(triangle.TexCoordsBegin(), triangle.TexCoordsEnd());
    }

    vector<vec4> Points;
    vector<vec4> Normals;
    vector<vec2> TexCoords;
};

vector<Triangle> Triangle::PartsInCube(const Cube& cube) const {

}
