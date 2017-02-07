#ifndef OCTREENODENEW_H
#define OCTREENODENEW_H

#include <array>

#include "OctreeBaseNode.h"
#include "OctreeLeaf.h"

class OctreeNodeNew : public OctreeBaseNode
{
    public:
        OctreeNodeNew() {}
        OctreeNodeNew(const VM::vec4& minPoint, const VM::vec4& maxPoint, const uint depth):
            OctreeBaseNode(minPoint, maxPoint),
            Depth(depth)
        {
            Subnodes.fill(nullptr);
        }

        ///Getters
        virtual std::vector<Patch> GetPatches() const;
        virtual std::vector<Patch> GetPatches(const Volume& volume) const;
        virtual std::vector<uint> GetTriangles() const;
        virtual std::vector<Vertex> GetVertices() const;
        virtual int GetDepth() const;

        ///Setters
        virtual void SetPatchesIndices(uint& index);
        virtual void SetPatchesIndices(std::vector<uint>::const_iterator& indices);

        ///Other functions
        virtual void AddTriangles(const std::vector<Triangle>& triangles);
        virtual bool NodeIsEmpty(const VM::ivec3& index) const;
        virtual void GeneratePatches(const OctreeBaseNode& root, const VM::ivec3& index);
        virtual void RemovePatchesByIndices(const std::vector<uint>& indices);
        virtual void GenerateRevertRelation(const OctreeBaseNode& root, const VM::ivec3& index);

        ///Operators
        virtual OctreeBaseNode& operator[](const VM::ivec3& index);
        virtual const OctreeBaseNode& operator[](const VM::ivec3& index) const;


        virtual ~OctreeNodeNew() {}
    protected:
    private:
        void ParseOctreeIndex(const VM::ivec3& oldIndex, VM::ivec3& newIndex, int& localIndex) const;

        std::array<OctreeBaseNode*, 8> Subnodes;
        uint Depth;
};

#endif // OCTREENODENEW_H
