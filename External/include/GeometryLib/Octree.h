#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <array>

#include "Utility.h"

#include "Polygon.h"

class Octree
{
	public:
		Octree();

        Octree(
			const VM::vec4& min_point,
			const VM::vec4& max_point,
			const uint side=1);

        void addPolygon(const TexturedPolygon& polygon);
        std::vector<TexturedPolygon> getPolygons() const;

        Octree* getSubtree(const VM::vec4& point);
        uint getNodesCount() const;
        inline uint getSide() const {return 1 << depth;}
        inline VM::vec4 getMinPoint() const {return min_point;}
        inline VM::vec4 getMaxPoint() const {return max_point;}
        inline uint getDepth() const {return depth;}
        VM::uvec3 getSubtreeIndex(const VM::vec4& point) const;
        bool pointInSubtree(const VM::vec4& point) const;
        std::vector<TexturedPolygon> getPatchesFromCube(const VM::vec4& min_point, const VM::vec4& max_point) const;

        Octree* operator[](const VM::uvec3& index);
		const Octree* operator[](const VM::uvec3& index) const;
	protected:
	private:
		bool leaf;
        uint depth;
        VM::vec4 min_point, max_point;
        std::vector<TexturedPolygon> polygons;
        std::array<Octree *, 8> subtrees;
};

Octree CreateVoxelTreeByScene(const Octree& scene);

class OctreeWithTriangles {
public:
private:

};

#endif // OCTREE_H
