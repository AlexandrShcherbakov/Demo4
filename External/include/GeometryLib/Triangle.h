#ifndef NEWTRIANGLE_H
#define NEWTRIANGLE_H

#include <array>
#include <vector>

#include "Utility.h"

#include "Vertex.h"

class Triangle
{
    public:
        static const uint PointsCount = 3;

        Triangle() {}
        Triangle(const uint A, const uint B, const uint C): Indices({A, B, C}) {}
        Triangle(const Vertex& A, const Vertex& B, const Vertex& C):
            Indices({GetVertexIndex(A), GetVertexIndex(B), GetVertexIndex(C)})
        {}
        Triangle(const std::array<Vertex, PointsCount>& points):
            Indices({
                GetVertexIndex(points[0]),
                GetVertexIndex(points[1]),
                GetVertexIndex(points[2])
            })
        {}

        ///Getters
        std::array<uint, PointsCount> GetIndices() const {
            return Indices;
        }
        std::vector<Vertex> GetVertices() const;
        VM::vec4 GetMeanNormal() const;
        float GetSquare() const;
        const GL::Image* GetImagePointer() const;
        std::vector<VM::vec2> GetTexCoords() const;
        VM::vec4 GetAmbientColor() const;
        Vertex GetVertexFromGlobal(const uint index) const;

        ///Setters
        void SetVertex(const Vertex& vertex, const uint index) {
            Indices[index] = GetVertexIndex(vertex);
        }
        void SetVertex(const uint vertexIndex, const uint index) {
            Indices[index] = vertexIndex;
        }
        void SetVertexRevertRelation(const uint index, const VM::uvec4& indices, const VM::vec4& weights);

        ///Other functions
        std::array<std::vector<Triangle>, 2> SplitByPlane(const VM::vec4& plane);

    protected:
    private:
        uint GetVertexIndex(const Vertex& vertex);
        uint GetMiddleIndex(const uint idx1, const uint idx2, const VM::vec4& plane);

        std::array<uint, PointsCount> Indices;
};

#endif // NEWTRIANGLE_H
