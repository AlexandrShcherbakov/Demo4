#ifndef OCTREENODE_H
#define OCTREENODE_H

#include <array>

#include "Triangle.h"
#include "Cube.h"
#include "CubeWithTriangles.h"


class OctreeNode : public Cube
{
	public:
		OctreeNode(const uint depth, const VM::vec4& minPoint, const VM::vec4& maxPoint);

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
