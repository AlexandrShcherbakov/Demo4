#ifndef OCTREEBASENODE_H
#define OCTREEBASENODE_H

#include <vector>

#include "Utility.h"

#include "Patch.h"
#include "NewTriangle.h"
#include "Vertex.h"
#include "Volume.h"

class OctreeBaseNode
{
    public:
        OctreeBaseNode() {}
        OctreeBaseNode(const VM::vec4& minPoint, const VM::vec4& maxPoint):
            MinPoint(minPoint),
            MaxPoint(maxPoint) {}

        ///Getters
        virtual std::vector<Patch> GetPatches() const = 0;
        virtual std::vector<Patch> GetPatches(const Volume& volume) const = 0;
        virtual std::vector<uint> GetTriangles() const = 0;
        virtual std::vector<Vertex> GetVertices() const = 0;
        virtual int GetDepth() const = 0;
        VM::vec4 GetMinPoint() const {
            return MinPoint;
        }
        VM::vec4 GetMaxPoint() const {
            return MaxPoint;
        }

        ///Setters
        virtual void SetPatchesIndices(uint& index) = 0;

        ///Other functions
        virtual void AddTriangles(const std::vector<Triangle>& triangles) = 0;
        virtual bool NodeIsEmpty(const VM::ivec3& index) const = 0;
        virtual void GeneratePatches(const OctreeBaseNode& root, const VM::ivec3& index) = 0;
        virtual void RemovePatchesByIndices(const std::vector<uint>& indices) = 0;
        virtual void GenerateRevertRelation(const OctreeBaseNode& root, const VM::ivec3& index) = 0;

        ///Operators
        virtual OctreeBaseNode& operator[](const VM::ivec3& index) = 0;
        virtual const OctreeBaseNode& operator[](const VM::ivec3& index) const = 0;


        virtual ~OctreeBaseNode() {}
    protected:
        VM::vec4 MinPoint;
        VM::vec4 MaxPoint;
    private:
};

class OctreeLeaf;
class OctreeNodeNew;

#endif // OCTREEBASENODE_H
