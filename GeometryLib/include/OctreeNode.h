#ifndef OCTREENODE_H
#define OCTREENODE_H

#include <array>

#include "Triangle.h"
#include "Cube.h"
#include "CubeWithTriangles.h"


class OctreeNode : public Cube
{
	public:
		OctreeNode() {};
		OctreeNode(const uint depth, const VM::vec4& minPoint, const VM::vec4& maxPoint);

		std::vector<VM::vec4> GetPoints() const;
        std::vector<VM::vec4> GetNormals() const;
        std::vector<VM::vec2> GetTexCoords() const;
        std::vector<uint> GetIndices() const;
        std::vector<uint> GetMaterialNumbers() const;
        std::vector<VM::vec4> GetAmbientColors() const;

        inline bool IsLeaf() const {
            return Subnodes == nullptr;
        }
        inline bool IsEmpty() const {
            return Subnodes == nullptr;
        }

        const Cube* operator[](const VM::uvec3& index) const;

		void AddTriangle(const Triangle& triangle);
		void AddTriangles(
			const std::vector<Triangle>::iterator& begin,
			const std::vector<Triangle>::iterator& end);
	protected:
	private:
        uint Depth;
        std::array<Cube*, 8> * Subnodes;
};

#endif // OCTREENODE_H
