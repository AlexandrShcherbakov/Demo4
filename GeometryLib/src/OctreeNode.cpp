#include "OctreeNode.h"

using namespace std;
using namespace VM;

OctreeNode::OctreeNode(const uint depth, const VM::vec4& minPoint, const VM::vec4& maxPoint) {
    this->MinPoint = minPoint;
    this->MaxPoint = maxPoint;
    this->Depth = depth;
    this->Subnodes = nullptr;
}

void OctreeNode::AddTriangle(const Triangle& triangle) {
    auto triangles = this->PartsInCube(triangle);
    if (triangles.empty()) {
        return;
    }
    if (this->Subnodes == nullptr) {
        this->Subnodes = new array<Cube*, 8>();
        Subnodes->fill(nullptr);
        vec4 center = (this->MinPoint + this->MaxPoint) / 2;
        for (uint i = 0; i < this->Subnodes->size(); ++i) {
            vec4 minPoint(
                (i & 4) ? center.x : this->MinPoint.x,
                (i & 2) ? center.y : this->MinPoint.y,
                (i & 1) ? center.z : this->MinPoint.z,
                1
            );
            vec4 maxPoint(
                (i & 4) ? this->MaxPoint.x : center.x,
                (i & 2) ? this->MaxPoint.y : center.y,
                (i & 1) ? this->MaxPoint.z : center.z,
                1
            );
            if (this->Depth) {
                (*this->Subnodes)[i] = new OctreeNode(this->Depth - 1, minPoint, maxPoint);
            } else {
                (*this->Subnodes)[i] = new CubeWithTriangles(minPoint, maxPoint);
            }
        }
    }
    for (uint i = 0; i < this->Subnodes->size(); ++i) {
        (*this->Subnodes)[i]->AddTriangles(triangles.begin(), triangles.end());
    }
}

void OctreeNode::AddTriangles(
    const vector<Triangle>::iterator& begin,
    const vector<Triangle>::iterator& end)
{
    for (auto iter = begin; iter != end; ++iter) {
        this->AddTriangle(*iter);
    }
}

vector<vec4> OctreeNode::GetPoints() const {
    vector<vec4> points;
    for (auto& subnode: *(this->Subnodes)) {
        auto subnodePoints = subnode->GetPoints();
        points.insert(points.end(), subnodePoints.begin(), subnodePoints.end());
    }
    return points;
}

vector<vec4> OctreeNode::GetNormals() const {
    vector<vec4> normals;
    for (auto& subnode: *(this->Subnodes)) {
        auto subnodeNormals = subnode->GetNormals();
        normals.insert(normals.end(), subnodeNormals.begin(), subnodeNormals.end());
    }
    return normals;
}

vector<vec2> OctreeNode::GetTexCoords() const {
    vector<vec2> texCoords;
    for (auto& subnode: *(this->Subnodes)) {
        auto subnodeTexCoords = subnode->GetTexCoords();
        texCoords.insert(texCoords.end(), subnodeTexCoords.begin(), subnodeTexCoords.end());
    }
    return texCoords;
}

vector<uint> OctreeNode::GetIndices() const {
    vector<uint> indices;
    for (auto& subnode: *(this->Subnodes)) {
        auto subnodeIndices = subnode->GetIndices();
        indices.insert(indices.end(), subnodeIndices.begin(), subnodeIndices.end());
    }
    return indices;
}

vector<uint> OctreeNode::GetMaterialNumbers() const {
    vector<uint> materialNumbers;
    for (auto& subnode: *(this->Subnodes)) {
        auto subnodeMaterialNumbers = subnode->GetMaterialNumbers();
        materialNumbers.insert(
            materialNumbers.end(),
            subnodeMaterialNumbers.begin(),
            subnodeMaterialNumbers.end());
    }
    return materialNumbers;
}

vector<vec4> OctreeNode::GetAmbientColors() const {
    vector<vec4> ambientColors;
    for (auto& subnode: *(this->Subnodes)) {
        auto subnodeAmbientColors = subnode->GetAmbientColors();
        ambientColors.insert(
            ambientColors.end(),
            subnodeAmbientColors.begin(),
            subnodeAmbientColors.end());
    }
    return ambientColors;
}

const Cube* OctreeNode::operator[](const VM::uvec3& index) const {
    if (this->IsLeaf()) {
        return this;
    }
    uvec3 indexInSubnode = index;
    uint subnodeIndex = 0;
    uint halfSide = 1 << this->Depth;
    for (uint i = 0; i < 3; ++i) {
        if (index[i] >= halfSide) {
            subnodeIndex += 1 << (2 - i);
            indexInSubnode[i] -= halfSide;
        }
    }
    if (this->Depth) {
        Cube* subnode = (*this->Subnodes)[subnodeIndex];
        return (*subnode)[indexInSubnode];
    }
    return (*this->Subnodes)[subnodeIndex];
}

void OctreeNode::SetIndices(uint& index) {
    for (uint i = 0; i < Subnodes->size(); ++i)
        (*Subnodes)[i]->SetIndices(index);
}

void OctreeNode::CreateFromTriangles(
    const Cube& octree,
    const Cube& node,
    const uvec3& index,
    const uint side)
{
    this->MinPoint = node.GetMinPoint();
    this->MaxPoint = node.GetMaxPoint();
    this->Depth = node.Depth;
    if (node.IsEmpty()) {
        return;
    }
    this->Subnodes = new array<Cube*, 8>();
    Subnodes->fill(nullptr);
    for (uint i = 0; i < 8; ++i) {
        uvec3 subindex = index * 2;
        for (uint j = 0; j < 3; ++j) {
			if (i & (1 << (2 - j))) {
				subindex[j] = subindex[j] + 1;
			}
        }
        if (!node.Depth) {
            (*Subnodes)[i] = new CubeWithPatches();
        } else {
            (*Subnodes)[i] = new OctreeNode();
        }
        (*Subnodes)[i]->CreateFromTriangles(octree, *((*(node.Subnodes))[i]), subindex, side);
    }
}

vector<Triangle> OctreeNode::GetTriangles() const {
    vector<Triangle> triangles;
    if (Subnodes == nullptr) {
		return triangles;
    }
    for (auto subnode: *(this->Subnodes)) {
        auto subnodeTriangles = subnode->GetTriangles();
        triangles.insert(
            triangles.end(),
            subnodeTriangles.begin(),
            subnodeTriangles.end());
    }
    return triangles;
}

vector<Patch> OctreeNode::GetPatches(const Volume* volume) const {
    vector<Patch> patches;
    if (this->IsEmpty()) {
		return patches;
    }
    for (auto subnode: *(this->Subnodes)) {
		if (volume != nullptr && !volume->IntersectsWithCube(subnode->GetMinPoint(), subnode->GetMaxPoint())) {
			continue;
		}
        auto subnodePatches = subnode->GetPatches(volume);
        patches.insert(
            patches.end(),
            subnodePatches.begin(),
            subnodePatches.end());
    }
    return patches;
}

void OctreeNode::ReorganizeTriangles() {
	if (this->IsEmpty()) {
		return;
    }
    for (auto subnode: *(this->Subnodes)) {
        subnode->ReorganizeTriangles();
    }
}
