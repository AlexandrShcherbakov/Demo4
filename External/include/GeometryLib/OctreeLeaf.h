#ifndef OCTREELEAF_H
#define OCTREELEAF_H

#include "OctreeBaseNode.h"


class OctreeLeaf : public OctreeBaseNode
{
    public:
        OctreeLeaf(const VM::vec4& minPoint, const VM::vec4& maxPoint):
            OctreeBaseNode(minPoint, maxPoint) {}

        ///Getters
        virtual std::vector<Patch> GetPatches() const;
        virtual std::vector<Patch> GetPatches(const Volume& volume) const;
        virtual std::vector<uint> GetTriangles() const;
        virtual std::vector<Vertex> GetVertices() const;
        virtual int GetDepth() const;

        ///Setters
        virtual void SetPatchesIndices(uint& index);

        ///Other functions
        virtual void AddTriangles(const std::vector<Triangle>& triangles);
        virtual bool NodeIsEmpty(const VM::ivec3& index) const;
        virtual void GeneratePatches(const OctreeBaseNode& root, const VM::ivec3& index);
        virtual void RemovePatchesByIndices(const std::vector<uint>& indices);
        virtual void GenerateRevertRelation(const OctreeBaseNode& root, const VM::ivec3& index);

        ///Operators
        virtual OctreeBaseNode& operator[](const VM::ivec3& index);
        virtual const OctreeBaseNode& operator[](const VM::ivec3& index) const;

        virtual ~OctreeLeaf() {}
    protected:
    private:
        void AddPatch(const OctreeBaseNode& root, const VM::ivec3& index, const VM::ivec3& normal);
        void AddPatch(const VM::vec4& normal);

        std::vector<Patch> Patches;
        std::vector<Triangle> Triangles;
};

#endif // OCTREELEAF_H
