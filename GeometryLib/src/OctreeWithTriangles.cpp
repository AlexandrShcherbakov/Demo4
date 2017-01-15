#include "Octree.h"

#include "Triangle.h"

using namespace GL;
using namespace std;
using namespace VM;

OctreeWithTriangles::OctreeWithTriangles(
	const uint side,
	const vec4& minPoint,
	const vec4& maxPoint)
{
    uint depth = 0;
    while ((1u << depth) < side + 2)
		depth += 1;
	vec4 center = (minPoint + maxPoint) / 2;
	float radius = max(maxPoint - minPoint) / side * (1u << (depth - 1)) - VEC_EPS;
    vec4 radiusvec(radius, radius, radius, 0);
    Root = OctreeNode(depth, center - radiusvec, center + radiusvec);
    Side = side;
}

void OctreeWithTriangles::SetTriangle(
	const vec4* points,
	const vec4* normals,
	const vec2* texCoords,
	const Image* image,
	const vec4& color,
	const uint materialNumber)
{
    Triangle triangle;
    triangle.SetAmbientColor(color);
    triangle.SetImagePointer(image);
    triangle.SetMaterialNumber(materialNumber);
    triangle.SetPoints(points, normals, texCoords);
    Root.AddTriangle(triangle);
}
