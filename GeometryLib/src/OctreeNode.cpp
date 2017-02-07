#include "OctreeNode.h"

std::vector<Patch> OctreeNodeNew::GetPatches() const {
    std::vector<Patch> result;
    for (auto subnode: Subnodes) {
        if (subnode != nullptr) {
            Append(result, subnode->GetPatches());
        }
    }
    return result;
}

std::vector<Patch> OctreeNodeNew::GetPatches(const Volume& volume) const {
    std::vector<Patch> result;
    for (auto subnode: Subnodes) {
        if (subnode != nullptr && volume.IntersectsWithCube(subnode->GetMinPoint(), subnode->GetMaxPoint())) {
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
    for (auto subnode: Subnodes) {
        if (subnode != nullptr) {
            return subnode->GetVertices();
        }
    }
    return std::vector<Vertex>();
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
        auto subTriangles = triangle.SplitByPlane(VM::vec4(1, 0, 0, -center.x));
        Append(splitedByX[0], subTriangles[0]);
        Append(splitedByX[1], subTriangles[1]);
    }
    for (uint x = 0; x < 2; ++x) {
        for (auto triangle: splitedByX[x]) {
            auto subTriangles = triangle.SplitByPlane(VM::vec4(0, 1, 0, -center.y));
            Append(splitedByY[x * 2 + 0], subTriangles[0]);
            Append(splitedByY[x * 2 + 1], subTriangles[1]);
        }
    }
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            for (auto triangle: splitedByY[x * 2 + y]) {
                auto subTriangles = triangle.SplitByPlane(VM::vec4(0, 0, 1, -center.z));
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
                if (Depth > 1) {
                    Subnodes[i] = new OctreeNodeNew(minPoint, maxPoint, Depth - 1);
                } else {
                    Subnodes[i] = new OctreeLeaf(minPoint, maxPoint);
                }
                Subnodes[i]->AddTriangles(splitedByZ[i]);
            }
        }
    }
}

void OctreeNodeNew::ParseOctreeIndex(
    const VM::ivec3& oldIndex,
    VM::ivec3& newIndex,
    int& localIndex
) const {
    int side = 1 << (Depth - 1);
    newIndex = VM::ivec3(0);
    localIndex = 0;
    for (uint i = 0; i < 3; ++i) {
        localIndex <<= 1;
        newIndex[i] = oldIndex[i];
        if (oldIndex[i] >= side) {
            localIndex++;
            newIndex[i] -= side;
        }
    }
}

bool OctreeNodeNew::NodeIsEmpty(const VM::ivec3& index) const {
    int subnodeIndex;
    VM::ivec3 indexInSubnode;
    ParseOctreeIndex(index, indexInSubnode, subnodeIndex);
    if (Subnodes[subnodeIndex] == nullptr) {
        return true;
    }

    return Subnodes[subnodeIndex]->NodeIsEmpty(indexInSubnode);
}

void OctreeNodeNew::GeneratePatches(const OctreeBaseNode& root, const VM::ivec3& index) {
    for (uint i = 0; i < Subnodes.size(); ++i) {
        if (Subnodes[i] != nullptr) {
            VM::ivec3 shift((i & 4) >> 2, (i & 2) >> 1, i & 1);
            Subnodes[i]->GeneratePatches(root, index * 2 + shift);
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

void OctreeNodeNew::GenerateRevertRelation(const OctreeBaseNode& root, const VM::ivec3& index) {
    for (uint i = 0; i < Subnodes.size(); ++i) {
        if (Subnodes[i] != nullptr) {
            VM::ivec3 shift((i & 4) >> 2, (i & 2) >> 1, i & 1);
            Subnodes[i]->GenerateRevertRelation(root, index * 2 + shift);
        }
    }
}

const OctreeBaseNode& OctreeNodeNew::operator[](const VM::ivec3& index) const {
    int subnodeIndex;
    VM::ivec3 indexInSubnode;
    ParseOctreeIndex(index, indexInSubnode, subnodeIndex);
    if (Subnodes[subnodeIndex] == nullptr) {
        throw "Access to empty octree node";
    }

    return (*Subnodes[subnodeIndex])[indexInSubnode];
}

OctreeBaseNode& OctreeNodeNew::operator[](const VM::ivec3& index) {
    return const_cast<OctreeBaseNode&>(
        static_cast<const OctreeNodeNew&>(*this)[index]
    );
}

int OctreeNodeNew::GetDepth() const {
    return Depth;
}

void OctreeNodeNew::SetPatchesIndices(std::vector<uint>::const_iterator& indices) {
        for (auto subnode: Subnodes) {
        if (subnode != nullptr) {
            subnode->SetPatchesIndices(indices);
        }
    }
}
