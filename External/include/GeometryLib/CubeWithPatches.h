#ifndef CUBEWITHPATCHES_H
#define CUBEWITHPATCHES_H

#include <array>

#include "Utility.h"

#include "Cube.h"
#include "OctreeNode.h"
#include "Patch.h"


class CubeWithPatches : public Cube
{
	public:
		CubeWithPatches();
		CubeWithPatches(
			const OctreeNode& octree,
			const VM::uvec3& index,
			const uint side);

		std::vector<VM::vec4> GetPoints() const;
        std::vector<VM::vec4> GetNormals() const;
        std::vector<VM::vec2> GetTexCoords() const;
        std::vector<uint> GetIndices() const;
        std::vector<uint> GetMaterialNumbers() const;
        std::vector<VM::vec4> GetAmbientColors() const;

        inline bool IsLeaf() const {
        	return true;
        }
        inline bool IsEmpty() const {
            return Patches.empty();
        }

        void AddTriangle(const Triangle& triangle);
        void AddTriangles(
			const std::vector<Triangle>::iterator& begin,
			const std::vector<Triangle>::iterator& end);

	protected:
	private:
        std::vector<Patch> Patches;
        std::vector<uint> Indices;

		void AddPatch(
			const OctreeNode& octree,
			const VM::uvec3& index,
			const uint side,
			const VM::vec4& normal);

		void AddPatch(
			const std::vector<Triangle>& triangles,
			const std::vector<uint>& indices,
			const VM::vec4& normal);
};

#endif // CUBEWITHPATCHES_H
