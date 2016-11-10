#ifndef OCTREENODE_H
#define OCTREENODE_H

#include <array>

#include "Triangle.h"
#include "Cube.h"
#include "CubeWithPatches.h"
#include "CubeWithTriangles.h"


class OctreeNode : public Cube
{
	public:
		OctreeNode() {
			Subnodes = nullptr;
		}
		OctreeNode(const uint depth, const VM::vec4& minPoint, const VM::vec4& maxPoint);

		std::vector<VM::vec4> GetPoints() const;
        std::vector<VM::vec4> GetNormals() const;
        std::vector<VM::vec2> GetTexCoords() const;
        std::vector<uint> GetIndices() const;
        std::vector<uint> GetMaterialNumbers() const;
        std::vector<VM::vec4> GetAmbientColors() const;
        std::vector<Triangle> GetTriangles() const;
        std::vector<Patch> GetPatches(const Volume* volume=nullptr) const;

        void CreateFromTriangles(
			const Cube& octree,
			const Cube& node,
			const VM::uvec3& index,
			const uint side);

		void SetIndices(uint& index);

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

		void ReorganizeTriangles();

		void RemovePatch(const Patch& patch);

	protected:
	private:

};

#endif // OCTREENODE_H
