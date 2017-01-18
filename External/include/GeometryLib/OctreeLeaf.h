#ifndef OCTREELEAF_H
#define OCTREELEAF_H

#include "OctreeBaseNode.h"


class OctreeLeaf : public OctreeBaseNode
{
    public:
        OctreeLeaf(const VM::vec4& minPoint, const VM::vec4& maxPoint):
            OctreeBaseNode(minPoint, maxPoint) {}

        ///Getters
        virtual std::vector<Patch> GetPatches() const = 0;
        virtual std::vector<uint> GetTriangles() const = 0;
        virtual std::vector<Vertex> GetVertices() const = 0;

        ///Setters
        virtual void SetPatchesIndices(uint& index) = 0;

        ///Other functions
        virtual void AddTriangles(const std::vector<Triangle>& triangles) = 0;
        virtual bool NodeIsEmpty(const VM::uvec3& index) const = 0;
        virtual void GeneratePatches() = 0;
        virtual void RemovePatchesByIndices(const std::vector<uint>& indices) = 0;
        virtual void GenerateRevertRelation() = 0;

        ///Operators
        virtual OctreeBaseNode& operator[](const VM::uvec3& index) = 0;
        virtual const OctreeBaseNode& operator[](const VM::uvec3& index) const = 0;

        virtual ~OctreeLeaf() {}
    protected:
    private:
};

#endif // OCTREELEAF_H
