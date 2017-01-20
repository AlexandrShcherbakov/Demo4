#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <array>

#include "Utility.h"

#include "OctreeNodeNew.h"

class Octree
{
	public:
        Octree(const VM::vec4& minPoint, const VM::vec4& maxPoint, const uint side);

        ///Getters
        std::vector<Patch> GetPatches() const {
            return Root.GetPatches();
        }
        std::vector<uint> GetTrianglesIndices() const {
            return Root.GetTriangles();
        }
        std::vector<Vertex> GetVertices() const {
            return Root.GetVertices();
        }

        ///Setters

        ///Other functions
        void Init(
            const std::vector<VM::vec4>& points,
            const std::vector<VM::vec4>& normals,
            const std::vector<VM::vec2>& texCoords,
            const std::vector<uint>& materialNumbers,
            const std::vector<const GL::Image*>& imagePointers,
            const std::vector<VM::vec4>& ambientColors
        );
        void RemovePatchesByIndices(const std::vector<uint>& indices) {
            Root.RemovePatchesByIndices(indices);
            SetPatchesIndices();
        }
        void GenerateRevertRelation() {
            Root.GenerateRevertRelation(Root, VM::ivec3(0));
        }

	protected:
	private:
	    void AddTriangles(
            const std::vector<VM::vec4>& points,
            const std::vector<VM::vec4>& normals,
            const std::vector<VM::vec2>& texCoords,
            const std::vector<uint>& materialNumbers,
            const std::vector<const GL::Image*>& imagePointers,
            const std::vector<VM::vec4>& ambientColors
        );
        void CreatePatches() {
            Root.GeneratePatches(&Root, VM::ivec3(0));
        }
        void SetPatchesIndices() {
            uint index = 0;
            Root.SetPatchesIndices(index);
        }

        OctreeNodeNew Root;
};

#endif // OCTREE_H
