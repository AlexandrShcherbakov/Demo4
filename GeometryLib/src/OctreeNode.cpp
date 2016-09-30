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
		vec4 center = (this->MinPoint + this->MaxPoint) / 2;
        for (uint i = 0; i < this->Subnodes->size(); ++i) {
            vec4 minPoint(
				(i & 4) ? center.x : this->MaxPoint.x,
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
            (*this->Subnodes)[i]->AddTriangles(triangles.begin(), triangles.end());
        }
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
