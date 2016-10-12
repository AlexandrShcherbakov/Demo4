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

        void SetIndices(uint& index);

        inline bool IsLeaf() const {
        	return true;
        }
        inline bool IsEmpty() const {
            return this->Triangles.empty();
        }

		const Cube* operator[](const VM::uvec3& index) const;

        inline void AddTriangle(const Triangle& triangle) {
            Triangles.push_back(triangle);
            Indices.push_back(Indices.size());
        }
        inline void AddTriangles(
			const std::vector<Triangle>::iterator& begin,
			const std::vector<Triangle>::iterator& end)
		{
            Triangles.insert(Triangles.end(), begin, end);
            for (uint i = Indices.size(); i < Triangles.size(); ++i) {
                Indices.push_back(i);
            }
        }

        void CreateFromTriangles(
			const Cube& octree,
			const VM::uvec3& index,
			const uint side);

        std::vector<Triangle> Triangles;
        std::vector<uint> Indices;
	protected:
	private:
};

#endif // CUBEWITHTRIANGLES_H
