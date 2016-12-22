#include "CubeWithPatches.h"

using namespace std;
using namespace VM;

CubeWithPatches::CubeWithPatches()
{
	//ctor
}

vector<vec4> CubeWithPatches::GetPoints() const {
	Unrealized(__FUNCTION__);
	return vector<vec4>();
}

vector<vec4> CubeWithPatches::GetNormals() const {
	Unrealized(__FUNCTION__);
	return vector<vec4>();
}

vector<vec2> CubeWithPatches::GetTexCoords() const {
	Unrealized(__FUNCTION__);
	return vector<vec2>();
}

vector<uint> CubeWithPatches::GetIndices() const {
	Unrealized(__FUNCTION__);
	return vector<uint>();
}

vector<uint> CubeWithPatches::GetMaterialNumbers() const {
	Unrealized(__FUNCTION__);
	return vector<uint>();
}

vector<vec4> CubeWithPatches::GetAmbientColors() const {
	Unrealized(__FUNCTION__);
	return vector<vec4>();
}

void CubeWithPatches::AddTriangle(const Triangle& triangle) {
	Unrealized(__FUNCTION__);
}

void CubeWithPatches::AddTriangles(
	const vector<Triangle>::iterator& begin,
	const vector<Triangle>::iterator& end)
{
	Unrealized(__FUNCTION__);
}

vec4 ColorFromOneTriangle(const Triangle& triangle) {
    if (triangle.ImagePointer != nullptr) {
    	const GL::Image * img = triangle.ImagePointer;
		vector<vec2> texCoords(triangle.TexCoordsBegin(), triangle.TexCoordsEnd());
        vec4 texColor = img->getAverageColor(texCoords);
        return texColor * triangle.AmbientColor;
    }
    return triangle.AmbientColor * 256.0f;
}

vec4 ComputeColorForPatch(const vector<Triangle>& triangles, const vec4& normal, const float patchSquare) {
    vec4 color(0, 0, 0, 0);
    float square = 0.0f;
    for (auto& triangle: triangles) {
        float angle = dot(triangle.MeanNormal(), normal);
        if (angle <= 0)
			continue;
        float p_square = triangle.GetSquare() * angle;
        square += p_square;
        color += ColorFromOneTriangle(triangle) * p_square;
    }
    return color / max(square, patchSquare) / 256.0f;
}

void CubeWithPatches::AddPatch(
	const vector<Triangle>& triangles,
	const vector<uint>& indices,
	const vec4& normal)
{
    vec4 center = (MaxPoint + MinPoint) / 2;
    float radius = MaxPoint.x - MinPoint.x;
    vector<vec4> points;
    for (uint i = 0; i < 8; ++i) {
        vec4 shift(0, 0, 0, 0);
        for (uint axis = 0; axis < 3; ++axis)
            shift[axis] += radius / 2 * ((i & (1 << (2 - axis))) ? 1 : -1);
        if (dot(normal, shift) < -VEC_EPS)
			continue;
		points.push_back(center + shift);
    }
    swap(points[2], points[3]);
    Patch patch;
    for (uint i = 0; i < points.size(); ++i)
		patch.Points[i] = points[i];
	vector<Triangle> filtered;
	for (uint i = 0; i < triangles.size(); ++i) {
        if(dot(triangles[i].MeanNormal(), normal) <= 0)
			continue;
        filtered.push_back(triangles[i]);
	}
    if (filtered.empty()) {
        return;
    }
	patch.Color = ComputeColorForPatch(filtered, normal, patch.GetSquare());
    patch.Normal = normal;
	Patches.push_back(patch);
}

void CubeWithPatches::AddPatch(
	const Cube& octree,
	const VM::uvec3& index,
	const uint side,
	const VM::vec4& normal)
{
	vec3 diff_coord(normal.x + (float)index.x, normal.y + (float)index.y, normal.z + (float)index.z);
	bool needs_to_draw = false;
	if (!needs_to_draw) {
		uvec3 new_coord((uint)diff_coord.x, (uint)diff_coord.y, (uint)diff_coord.z);
		const Cube * near_cube = octree[new_coord];
		needs_to_draw = near_cube->IsEmpty();
	}
	if (needs_to_draw) {
		const CubeWithTriangles * old_cube = (const CubeWithTriangles *)octree[index];
		this->AddPatch(old_cube->Triangles, old_cube->Indices, normal);
	}
}

void CubeWithPatches::CreateFromTriangles(
	const Cube& octree,
	const Cube& node,
	const uvec3& index,
	const uint side)
{
    MinPoint = octree[index]->GetMinPoint();
    MaxPoint = octree[index]->GetMaxPoint();
    AddPatch(octree, index, side, vec4(-1,  0,  0, 0));
    AddPatch(octree, index, side, vec4( 1,  0,  0, 0));
    AddPatch(octree, index, side, vec4( 0, -1,  0, 0));
    AddPatch(octree, index, side, vec4( 0,  1,  0, 0));
    AddPatch(octree, index, side, vec4( 0,  0, -1, 0));
    AddPatch(octree, index, side, vec4( 0,  0,  1, 0));
}

void CubeWithPatches::SetIndices(uint& index) {
    for (uint i = 0; i < Patches.size(); ++i)
		Patches[i].Index = index++;
}

const Cube* CubeWithPatches::operator[](const VM::uvec3& index) const {
    Unrealized(__FUNCTION__);
    return nullptr;
}

vector<Vertex> CubeWithPatches::GetVertices() const {
    Unrealized(__FUNCTION__);
    return vector<Vertex>();
}

vector<Triangle> CubeWithPatches::GetTriangles() const {
    Unrealized(__FUNCTION__);
    return vector<Triangle>();
}

void CubeWithPatches::ReorganizeTriangles() {
    Unrealized(__FUNCTION__);
}

vector<Patch> CubeWithPatches::GetPatches(const Volume* volume) const {
    if (volume == nullptr) {
        return Patches;
    }
    vector<Patch> patches;
    for (auto& p: Patches) {
        bool flag = false;
        for (uint i = 0; i < p.Points.size(); ++i) {
			flag |= volume->IncludesPoint(p.Points[i]);
        }
        if (flag) {
            patches.push_back(p);
        }
    }
    return patches;
}

void CubeWithPatches::RemovePatch(const Patch& patch) {
    for (uint i = 0; i < Patches.size(); ++i) {
        if (Patches[i] == patch) {
            Patches.erase(Patches.begin() + i);
            return;
        }
    }
}
