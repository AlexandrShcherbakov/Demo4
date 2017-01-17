#ifndef OCTREENODENEW_H
#define OCTREENODENEW_H

#include <array>

#include "OctreeBaseNode.h"

class OctreeNodeNew : public OctreeBaseNode
{
    public:
        OctreeNodeNew() {}
        OctreeNodeNew(const VM::vec4& minPoint, const VM::vec4& maxPoint, const uint depth):
            OctreeBaseNode(minPoint, maxPoint),
            Depth(depth) {}

        ///Getters
        virtual std::vector<Patch> GetPatches() const;
        virtual std::vector<uint> GetTriangles() const;
        virtual std::vector<Vertex> GetVertices() const;

        ///Setters
        virtual void SetPatchesIndices(uint& index);

        ///Other functions
        virtual void AddTriangles(const std::vector<Triangle>& triangles);
        virtual bool NodeIsEmpty(const VM::uvec3& index) const;
        virtual void GeneratePatches();
        virtual void RemovePatchesByIndices(const std::vector<uint>& indices);
        virtual void GenerateRevertRelation();

        ///Operators
        virtual OctreeBaseNode& operator[](const VM::uvec3& index);
        virtual const OctreeBaseNode& operator[](const VM::uvec3& index) const;


        virtual ~OctreeNodeNew() {}
    protected:
    private:
        std::array<OctreeBaseNode*, 8> Subnodes;

        uint Depth;

};

template<typename T>
inline void Append(std::vector<T>& head, const std::vector<T>& tail) {
    head.insert(head.end(), tail.begin(), tail.end());
}

#endif // OCTREENODENEW_H
