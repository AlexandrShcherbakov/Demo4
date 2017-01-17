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

	    ///Getters

        ///Setters
        void SetVertex(const Vertex& vertex, const uint index) {
            Indices[index] = GetVertexIndex(vertex);
        }
        void SetVertex(const uint vertexIndex, const uint index) {
            Indices[index] = vertexIndex;
        }

        ///Other functions
        std::array<std::vector<Triangle>, 2> SplitByPlane(const VM::vec4& plane);

	protected:
	private:
        uint GetVertexIndex(const Vertex& vertex);
	    uint GetMiddleIndex(const uint idx1, const uint idx2, const VM::vec4& plane);

	    std::array<uint, PointsCount> Indices;

	    static std::vector<Vertex> Vertices;
	    static std::map<std::pair<uint, uint>, uint> MiddleIndices;
};

template<typename T>
inline T Interpolate(const T& p1, const T& p2, const float t) {
    return p1 + (p2 - p1) * t;
}

#endif // NEWTRIANGLE_H
