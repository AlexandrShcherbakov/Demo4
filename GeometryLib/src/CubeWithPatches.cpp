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
	return std::vector<uint>();
}

vector<vec4> CubeWithPatches::GetAmbientColors() const {
	Unrealized(__FUNCTION__);
	return vector<vec4>();
}

void CubeWithPatches::AddTriangle(const Triangle& triangle) {
	Unrealized(__FUNCTION__);
}

void CubeWithPatches::AddTriangles(
	const std::vector<Triangle>::iterator& begin,
	const std::vector<Triangle>::iterator& end)
{
	Unrealized(__FUNCTION__);
}
