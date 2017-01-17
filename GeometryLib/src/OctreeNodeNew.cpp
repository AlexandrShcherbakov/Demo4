#include "OctreeNodeNew.h"

std::vector<Patch> OctreeNodeNew::GetPatches() const {
    std::vector<Patch> result;
    for (auto subnode: Subnodes) {
        if (subnode != nullptr) {
            Append(result, subnode->GetPatches());
        }
    }
    return result;
}

std::vector<uint> OctreeNodeNew::GetTriangles() const {
    std::vector<uint> result;
    for (auto subnode: Subnodes) {
        if (subnode != nullptr) {
            Append(result, subnode->GetTriangles());
        }
    }
    return result;
}

std::vector<Vertex> OctreeNodeNew::GetVertices() const {
    std::vector<Vertex> result;
    for (auto subnode: Subnodes) {
        if (subnode != nullptr) {
            Append(result, subnode->GetVertices());
        }
    }
    return result;
}

void OctreeNodeNew::SetPatchesIndices(uint& index) {
    for (auto subnode: Subnodes) {
        if (subnode != nullptr) {
            subnode->SetPatchesIndices(index);
        }
    }
}

void OctreeNodeNew::AddTriangles(const std::vector<Triangle>& triangles) {
    std::array<std::vector<Triangle>, 2> splitedByX;
    std::array<std::vector<Triangle>, 4> splitedByY;
    std::array<std::vector<Triangle>, 8> splitedByZ;
    VM::vec4 center = (MinPoint + MaxPoint) / 2;

    for (auto triangle: triangles) {
        auto subTriangles = triangle.SplitByPlane(VM::vec4(1, 0, 0, center.x));
        Append(splitedByX[0], subTriangles[0]);
        Append(splitedByX[1], subTriangles[1]);
    }
    for (uint x = 0; x < 2; ++x) {
        for (auto triangle: splitedByX[x]) {
            auto subTriangles = triangle.SplitByPlane(VM::vec4(0, 1, 0, center.y));
            Append(splitedByY[x * 2 + 0], subTriangles[0]);
            Append(splitedByY[x * 2 + 1], subTriangles[1]);
        }
    }
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            for (auto triangle: splitedByY[x * 4 + y * 2]) {
                auto subTriangles = triangle.SplitByPlane(VM::vec4(0, 0, 1, center.z));
                Append(splitedByZ[x * 4 + y * 2 + 0], subTriangles[0]);
                Append(splitedByZ[x * 4 + y * 2 + 1], subTriangles[1]);
            }
        }
    }
    for (uint i = 0; i < Subnodes.size(); ++i) {
        if (splitedByZ[i].size()) {
            if (Subnodes[i] == nullptr) {
                VM::vec4 minPoint(
                    (i & 4) ? center.x : this->MinPoint.x,
                    (i & 2) ? center.y : this->MinPoint.y,
                    (i & 1) ? center.z : this->MinPoint.z,
                    1
                );
                VM::vec4 maxPoint(
                    (i & 4) ? this->MaxPoint.x : center.x,
                    (i & 2) ? this->MaxPoint.y : center.y,
                    (i & 1) ? this->MaxPoint.z : center.z,
                    1
                );
                if (Depth) {
                    Subnodes[i] = new OctreeNodeNew(minPoint, maxPoint, Depth - 1);
                }
                Subnodes[i]->AddTriangles(splitedByZ[i]);
            }
        }
    }
}

void OctreeNodeNew::ParseOctreeIndex(
    const VM::uvec3& oldIndex,
    VM::uvec3& newIndex,
    uint& localIndex
) const {
    uint side = 1 << (Depth - 1);
    newIndex = VM::uvec3(0u);
    localIndex = 0;
    for (uint i = 0; i < 3; ++i) {
        localIndex <<= 1;
        newIndex[i] = oldIndex[i];
        if (oldIndex[i] > side) {
            localIndex++;
            newIndex -= side;
        }
    }
}

bool OctreeNodeNew::NodeIsEmpty(const VM::uvec3& index) const {
    uint subnodeIndex;
    VM::uvec3 indexInSubnode;
    ParseOctreeIndex(index, indexInSubnode, subnodeIndex);
    if (Subnodes[subnodeIndex] == nullptr) {
        return true;
    }

    return Subnodes[subnodeIndex]->NodeIsEmpty(indexInSubnode);
}

void OctreeNodeNew::GeneratePatches() {
    for (auto subnode: Subnodes) {
        if (subnode != nullptr) {
            subnode->GeneratePatches();
        }
    }
}

void OctreeNodeNew::RemovePatchesByIndices(const std::vector<uint>& indices) {
    for (auto subnode: Subnodes) {
        if (subnode != nullptr) {
            subnode->RemovePatchesByIndices(indices);
        }
    }
}

void OctreeNodeNew::GenerateRevertRelation() {
    for (auto subnode: Subnodes) {
        if (subnode != nullptr) {
            subnode->GenerateRevertRelation();
        }
    }
}

const OctreeBaseNode& OctreeNodeNew::operator[](const VM::uvec3& index) const {
    uint subnodeIndex;
    VM::uvec3 indexInSubnode;
    ParseOctreeIndex(index, indexInSubnode, subnodeIndex);
    if (Subnodes[subnodeIndex] == nullptr) {
        throw "Access to empty octree node";
    }

    return (*Subnodes[subnodeIndex])[indexInSubnode];
}

OctreeBaseNode& OctreeNodeNew::operator[](const VM::uvec3& index) {
    return const_cast<OctreeBaseNode&>(
        static_cast<const OctreeNodeNew&>(*this)[index]
    );
}
