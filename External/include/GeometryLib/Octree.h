#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <array>

#include "Utility.h"

#include "Polygon.h"
#include "OctreeNode.h"

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
		OctreeWithTriangles(const uint side, const VM::vec4& minPoint, const VM::vec4& maxPoint);

        inline std::vector<VM::vec4> GetPoints() const {
            return Root.GetPoints();
        }
        inline std::vector<VM::vec4> GetNormals() const {
            return Root.GetNormals();
        }
        inline std::vector<VM::vec2> GetTexCoords() const {
            return Root.GetTexCoords();
        }
        inline std::vector<uint> GetMaterialNumbers() const {
            return Root.GetMaterialNumbers();
        }
        inline std::vector<VM::vec4> GetAmbientColors() const {
            return Root.GetAmbientColors();
        }

        inline std::vector<Triangle> GetTriangles() const {
            return Root.GetTriangles();
        }

        void SetTriangle(
			const VM::vec4* points,
			const VM::vec4* normals,
			const VM::vec2* texCoords,
			const GL::Image* image,
			const VM::vec4& color,
			const uint materialNumber);

        inline const Cube* operator[](const VM::uvec3& index) const {
            return Root[index];
        }

		OctreeNode Root;
        uint Side;
	private:
};

class OctreeWithPatches {
	public:
		OctreeWithPatches(OctreeWithTriangles& octree);

        inline const Cube* operator[](const VM::uvec3& index) const {
            return Root[index];
        }

		OctreeNode Root;
		uint Side;

		inline std::vector<Patch> GetPatches() const {
			return Root.GetPatches();
		}

        std::vector<std::vector<float> > CountFF() const;

        inline void RemovePatch(const Patch& patch) {
            Root.RemovePatch(patch);
        }

	private:
};

#endif // OCTREE_H
