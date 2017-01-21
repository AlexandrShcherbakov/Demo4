#include "NewTriangle.h"
#include <sstream>

struct VertexComparator {
    bool operator()(const Vertex& a, const Vertex& b) const {
        if (a.GetMaterialNumber() != b.GetMaterialNumber()) {
            return a.GetMaterialNumber() < b.GetMaterialNumber();
        }
        VM::vec4 posA = a.GetPosition();
        VM::vec4 posB = b.GetPosition();
        for (uint i = 0; i < 4; ++i) {
            if (posA[i] != posB[i]) {
                return posA[i] < posB[i];
            }
        }
        VM::vec4 normA = a.GetNormal();
        VM::vec4 normB = b.GetNormal();
        for (uint i = 0; i < 4; ++i) {
            if (normA[i] != normB[i]) {
                return normA[i] < normB[i];
            }
        }
        VM::vec2 texA = a.GetTexCoord();
        VM::vec2 texB = b.GetTexCoord();
        for (uint i = 0; i < 2; ++i) {
            if (texA[i] != texB[i]) {
                return texA[i] < texB[i];
            }
        }
        return false;
    }
};

static std::map<Vertex, uint, VertexComparator> UniqueVertices;
static std::vector<Vertex> Vertices;
static std::map<std::pair<uint, uint>, uint> MiddleIndices;

std::vector<Vertex> Triangle::GetVertices() const {
    return Vertices;
}

const GL::Image* Triangle::GetImagePointer() const {
    return Vertices[Indices[0]].GetImagePointer();
}

VM::vec4 Triangle::GetAmbientColor() const {
    return Vertices[Indices[0]].GetColor();
}

Vertex Triangle::GetVertexFromGlobal(const uint index) const {
    return Vertices[index];
}

void Triangle::SetVertexRevertRelation(const uint index, const VM::i16vec4& indices, const VM::vec4& weights) {
    Vertices[index].SetRelationIndices(indices);
    Vertices[index].SetRelationWeights(weights);
}

uint Triangle::GetVertexIndex(const Vertex& vertex) {
    if (!UniqueVertices.count(vertex)) {
        UniqueVertices[vertex] = Vertices.size();
        Vertices.push_back(vertex);
    }
    return UniqueVertices[vertex];
}

uint Triangle::GetMiddleIndex(const uint idx1, const uint idx2, const VM::vec4& plane) {
    auto edge = std::make_pair(std::min(idx1, idx2), std::max(idx1, idx2));
    if (MiddleIndices.count(edge)) {
        return MiddleIndices[edge];
    }

    Vertex vert1 = Vertices[idx1];
    Vertex vert2 = Vertices[idx2];
    VM::vec4 point1 = vert1.GetPosition();
    VM::vec4 point2 = vert2.GetPosition();
    VM::vec4 direction = point2 - point1;
    float t = -dot(point1, plane) / dot(direction, plane);
    Vertex vertex;
    vertex.SetPosition(Interpolate(point1, point2, t));
    vertex.SetNormal(Interpolate(vert1.GetNormal(), vert2.GetNormal(), t));
    vertex.SetTexCoord(Interpolate(vert1.GetTexCoord(), vert2.GetTexCoord(), t));
    vertex.SetMaterialNumber(vert1.GetMaterialNumber());
    vertex.SetImagePointer(vert1.GetImagePointer());
    vertex.SetColor(vert1.GetColor());
    return MiddleIndices[edge] = GetVertexIndex(vertex);
}

std::array<std::vector<Triangle>, 2> Triangle::SplitByPlane(const VM::vec4& plane) {
    std::vector<uint> left;
    std::vector<uint> right;

    for (auto idx: Indices) {
        if (VM::dot(Vertices[idx].GetPosition(), plane) < 0) {
            left.push_back(idx);
        } else {
            right.push_back(idx);
        }
    }

    bool swaped = left.size() < 2;
    if (swaped) {
        left.swap(right);
    }

    std::array<std::vector<Triangle>, 2> result;
    if (left.size() == 3) {
        result[0].push_back(Triangle(left[0], left[1], left[2]));
    } else {
        uint mid1 = GetMiddleIndex(left[0], right[0], plane);
        uint mid2 = GetMiddleIndex(left[1], right[0], plane);
        result[0].push_back(Triangle(left[0], left[1], mid1));
        result[0].push_back(Triangle(left[1], mid1, mid2));
        result[1].push_back(Triangle(right[0], mid1, mid2));
    }

    if (swaped) {
        result[0].swap(result[1]);
    }
    return result;
}

VM::vec4 Triangle::GetMeanNormal() const {
    VM::vec4 normal(0.0f);
    for (auto idx: Indices) {
        normal += Vertices[idx].GetNormal();
    }
    return normal / Indices.size();
}

float Triangle::GetSquare() const {
    VM::vec4 A = Vertices[Indices[0]].GetPosition();
    VM::vec4 B = Vertices[Indices[1]].GetPosition();
    VM::vec4 C = Vertices[Indices[2]].GetPosition();
    return VM::length(VM::cross(B - A, C - A)) / 2;
}

std::vector<VM::vec2> Triangle::GetTexCoords() const {
    std::vector<VM::vec2> result;
    for (auto idx: Indices) {
        result.push_back(Vertices[idx].GetTexCoord());
    }
    return result;
}
