#include "NewTriangle.h"

void NewTriangle::SetVertex(const Vertex& vertex, const uint index) {
    uint vertexIndex = 0;
    for (; vertexIndex < Vertices.size() && Vertices[vertexIndex] != vertex; ++vertexIndex) {}
    if (vertexIndex == Vertices.size()) {
        Vertices.push_back(vertex);
    }
    Indices[index] = vertexIndex;
}
