#ifndef OCTREEBASENODE_H
#define OCTREEBASENODE_H

#include <vector>

#include "Utility.h"

class Patch {};

class Triangle {};

class Vertex {};

class OctreeBaseNode
{
    public:
        ///Getters
        virtual std::vector<Patch> GetPatches() const = 0;
        virtual std::vector<uint> GetTriangles() const = 0;
        virtual std::vector<Vertex> GetVertices() const = 0;

        ///Setters
        virtual void SetPatchesIndices() = 0;

        ///Other functions
        virtual void AddTriangles(const std::vector<Triangle>& triangles) = 0;
        virtual bool NodeIsEmpty(const VM::uvec3& index) const = 0;
        virtual void GeneratePatches() = 0;
        virtual void RemovePatchesByIndices(const std::vector<uint> indices) = 0;
        virtual void GenerateRevertRelation() = 0;

        ///Operators
        virtual OctreeBaseNode& operator[](const VM::uvec3& index) = 0;
        virtual const OctreeBaseNode& operator[](const VM::uvec3& index) const = 0;


        virtual ~OctreeBaseNode();
    protected:
    private:
};

#endif // OCTREEBASENODE_H
