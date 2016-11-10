#ifndef CUBEWITHTRIANGLES_H
#define CUBEWITHTRIANGLES_H

#include <vector>

#include "Cube.h"
#include "Triangle.h"


class CubeWithTriangles : public Cube
{
	public:
		CubeWithTriangles(const VM::vec4& minPoint, const VM::vec4& maxPoint);

		std::vector<VM::vec4> GetPoints() const;
        std::vector<VM::vec4> GetNormals() const;
        std::vector<VM::vec2> GetTexCoords() const;
        std::vector<uint> GetMaterialNumbers() const;
        std::vector<VM::vec4> GetAmbientColors() const;
        inline std::vector<uint> GetIndices() const {
            return Indices;
        }
        inline std::vector<Triangle> GetTriangles() const {
            return Triangles;
        }
        std::vector<Patch> GetPatches(const Volume* volume) const;

        void SetIndices(uint& index);

        inline bool IsLeaf() const {
        	return true;
        }
        inline bool IsEmpty() const {
            return this->Triangles.empty();
        }

		const Cube* operator[](const VM::uvec3& index) const;

        inline void AddTriangle(const Triangle& triangle) {
        	auto triangles = PartsInCube(triangle);
        	AddTriangles(triangles.begin(), triangles.end());
        }
        inline void AddTriangles(
			const std::vector<Triangle>::iterator& begin,
			const std::vector<Triangle>::iterator& end)
		{
			for (auto it = begin; it != end; ++it) {
                auto triangles = PartsInCube(*it);
                Triangles.insert(Triangles.end(), triangles.begin(), triangles.end());
			}
            for (uint i = Indices.size(); i < Triangles.size(); ++i) {
                Indices.push_back(i);
            }
        }

        void CreateFromTriangles(
			const Cube& octree,
			const Cube& node,
			const VM::uvec3& index,
			const uint side);

		void ReorganizeTriangles();

		void RemovePatch(const Patch& patch);
	protected:
	private:
};

#endif // CUBEWITHTRIANGLES_H
