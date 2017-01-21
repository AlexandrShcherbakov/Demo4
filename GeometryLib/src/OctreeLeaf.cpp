#include "OctreeLeaf.h"

std::vector<Patch> OctreeLeaf::GetPatches() const {
    return Patches;
}
std::vector<Patch> OctreeLeaf::GetPatches(const Volume& volume) const {
    std::vector<Patch> result;
    for (Patch patch: Patches) {
        bool flag = false;
        for (uint i = 0; i < patch.GetPointsSize() && !flag; ++i) {
            flag = volume.IncludesPoint(patch.GetPoint(i));
        }
        if (flag) {
            result.push_back(patch);
        }
    }
    return result;
}

std::vector<uint> OctreeLeaf::GetTriangles() const {
    std::vector<uint> result;
    for (Triangle triangle: Triangles) {
        Append(result, triangle.GetIndices());
    }
    return result;
}

std::vector<Vertex> OctreeLeaf::GetVertices() const {
    return Triangles[0].GetVertices();
}

void OctreeLeaf::SetPatchesIndices(uint& index) {
    for (uint i = 0; i < Patches.size(); ++i) {
        Patches[i].SetIndex(index++);
    }
}

void OctreeLeaf::AddTriangles(const std::vector<Triangle>& triangles) {
    Triangles = triangles;
}

bool OctreeLeaf::NodeIsEmpty(const VM::ivec3& index) const {
    return Triangles.empty();
}

void OctreeLeaf::RemovePatchesByIndices(const std::vector<uint>& indices) {
    for (int i = Patches.size() - 1; i >= 0; --i) {
        if (std::binary_search(indices.begin(), indices.end(), Patches[i].GetIndex())) {
            Patches.erase(Patches.begin() + i);
        }
    }
}

int OctreeLeaf::GetDepth() const {
    return 0;
}

void OctreeLeaf::GeneratePatches(const OctreeBaseNode& root, const VM::ivec3& index) {
    for (uint i = 0; i < 3; ++i) {
        for (int j = -1; j <= 1; j += 2) {
            VM::ivec3 normal(0);
            normal[i] = j;
            AddPatch(root, index, normal);
        }
    }
}

void OctreeLeaf::AddPatch(
	const OctreeBaseNode& root,
	const VM::ivec3& index,
	const VM::ivec3& normal
) {
    int depth = root.GetDepth();
    int side = 1 << depth;
    VM::ivec3 neib = index + normal;
	if (VM::min(neib) == -1 || VM::max(neib) == side || root.NodeIsEmpty(neib)) {
		AddPatch(VM::vec4(normal.x, normal.y, normal.z, 0));
	}
}

VM::vec4 ColorFromOneTriangle(const Triangle& triangle) {
    if (triangle.GetImagePointer() != nullptr) {
    	const GL::Image * img = triangle.GetImagePointer();
		std::vector<VM::vec2> texCoords = triangle.GetTexCoords();
        VM::vec4 texColor = img->getAverageColor(texCoords);
        return texColor * triangle.GetAmbientColor();
    }
    return triangle.GetAmbientColor() * 256.0f;
}

VM::vec4 ComputeColorForPatch(const std::vector<Triangle>& triangles, const VM::vec4& normal, const float patchSquare) {
    VM::vec4 color(0, 0, 0, 0);
    float square = 0.0f;
    for (auto& triangle: triangles) {
        float angle = dot(triangle.GetMeanNormal(), normal);
        if (angle <= 0)
			continue;
        float p_square = triangle.GetSquare() * angle;
        square += p_square;
        color += ColorFromOneTriangle(triangle) * p_square;
    }
    return color / std::max(square, patchSquare) / 256.0f;
}

void OctreeLeaf::AddPatch(const VM::vec4& normal) {
    VM::vec4 center = (MaxPoint + MinPoint) / 2;
    float radius = MaxPoint.x - MinPoint.x;
    std::vector<VM::vec4> points;
    for (uint i = 0; i < 8; ++i) {
        VM::vec4 shift(0, 0, 0, 0);
        for (uint axis = 0; axis < 3; ++axis)
            shift[axis] += radius / 2 * ((i & (1 << (2 - axis))) ? 1 : -1);
        if (VM::dot(normal, shift) < -VEC_EPS)
			continue;
		points.push_back(center + shift);
    }
    std::swap(points[2], points[3]);
    Patch patch;
    for (uint i = 0; i < points.size(); ++i) {
        patch.SetPoint(i, points[i]);
    }
	std::vector<Triangle> filtered;
	for (uint i = 0; i < Triangles.size(); ++i) {
        if(VM::dot(Triangles[i].GetMeanNormal(), normal) <= 0)
			continue;
        filtered.push_back(Triangles[i]);
	}
    if (filtered.empty()) {
        return;
    }
    float square = patch.GetSquare();
	patch.SetColor(ComputeColorForPatch(filtered, normal, square));
    patch.SetNormal(normal);
	Patches.push_back(patch);
}

const OctreeBaseNode& OctreeLeaf::operator[](const VM::ivec3& index) const {
    if (index == VM::ivec3(0)) {
        return *this;
    }
    throw "Wrong index for OctreeLeaf";
}

OctreeBaseNode& OctreeLeaf::operator[](const VM::ivec3& index) {
    return const_cast<OctreeBaseNode&>(
        static_cast<const OctreeLeaf&>(*this)[index]
    );
}

void OctreeLeaf::GenerateRevertRelation(const OctreeBaseNode& root, const VM::ivec3& index) {
    std::set<uint> indices;
    for (uint i = 0; i < Triangles.size(); ++i) {
        auto vertIndices = Triangles[i].GetIndices();
        for (uint j = 0; j < vertIndices.size(); ++j) {
            if (Triangles[i].GetVertexFromGlobal(vertIndices[j]).GetRelationWeights().x < VEC_EPS) {
                indices.insert(vertIndices[j]);
            }
        }
    }

    std::vector<Patch> localPatches;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            for (int z = -1; z <= 1; ++z) {
                VM::ivec3 currentIndex = index + VM::ivec3(x, y, z);
                if (VM::min(currentIndex) == -1
                    || VM::max(currentIndex) == (1 << root.GetDepth())
                    || root.NodeIsEmpty(currentIndex)) {
                    continue;
                }
                Append(localPatches, root[currentIndex].GetPatches());
            }
        }
    }

    for (auto idx: indices) {
        Vertex vert = Triangles[0].GetVertexFromGlobal(idx);
        std::vector<std::pair<float, uint> > measure(localPatches.size());
        for (uint i = 0; i < localPatches.size(); ++i) {
            measure[i].first = std::max(VM::dot(vert.GetNormal(), localPatches[i].GetNormal()), 0.0f);
            measure[i].second = i;
        }
        std::sort(measure.rbegin(), measure.rend());
        VM::vec4 weights(0.0f);
        VM::i16vec4 relIndices;
        for (uint i = 0; i < std::min(4u, localPatches.size()); ++i) {
            relIndices[i] = static_cast<short>(localPatches[measure[i].second].GetIndex());
            weights[i] = measure[i].first;
        }
        Triangles[0].SetVertexRevertRelation(idx, relIndices, weights);
    }
}
