#include "Octree.h"
#include <fstream>

using namespace VM;

Octree::Octree(const vec4& minPoint, const vec4& maxPoint, const uint side) {
    uint depth = 0;
    while ((1u << depth) < (side + 2)) {
        depth++;
    }
    float diameter = VM::max(maxPoint - minPoint) + VEC_EPS;
    diameter = diameter / (side + 2) * (1 << (depth - 1));
    VM::vec4 center = (minPoint + maxPoint) / 2;
    VM::vec4 newMinPoint = center - VM::vec4(VM::vec3(diameter), 0.0f);
    VM::vec4 newMaxPoint = center + VM::vec4(VM::vec3(diameter), 0.0f);
    Root = OctreeNodeNew(newMinPoint, newMaxPoint, depth);
}

void Octree::Init(
    const std::vector<VM::vec4>& points,
    const std::vector<VM::vec4>& normals,
    const std::vector<VM::vec2>& texCoords,
    const std::vector<uint>& materialNumbers,
    const std::vector<const GL::Image*>& imagePointers,
    const std::vector<VM::vec4>& ambientColors
) {
    AddTriangles(points, normals, texCoords, materialNumbers, imagePointers, ambientColors);
    CreatePatches();
}

void Octree::AddTriangles(
    const std::vector<VM::vec4>& points,
    const std::vector<VM::vec4>& normals,
    const std::vector<VM::vec2>& texCoords,
    const std::vector<uint>& materialNumbers,
    const std::vector<const GL::Image*>& imagePointers,
    const std::vector<VM::vec4>& ambientColors
) {
    std::vector<Triangle> triangles;
    for (uint i = 0; i < points.size(); i += 3) {
        std::array<Vertex, 3> trianglePoints;
        for (uint j = 0; j < trianglePoints.size(); ++j) {
            trianglePoints[j] = Vertex(
                points[i + j], normals[i + j],
                ambientColors[materialNumbers[i + j]], texCoords[i + j],
                materialNumbers[i + j], imagePointers[materialNumbers[i + j]]
            );
        }
        triangles.push_back(Triangle(trianglePoints));
    }
    Root.AddTriangles(triangles);
}
