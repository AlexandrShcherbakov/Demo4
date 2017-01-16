#ifndef NEWTRIANGLE_H
#define NEWTRIANGLE_H

#include <array>
#include <vector>

#include "Utility.h"

#include "Vertex.h"

class NewTriangle
{
    public:
	    static const uint PointsCount = 3;

	    ///Getters

        ///Setters
        void SetVertex(const Vertex& vertex, const uint index);
        void SetVertex(const uint vertexIndex, const uint index) {
            Indices[index] = vertexIndex;
        }

	protected:
	private:
	    std::array<uint, PointsCount> Indices;

	    static std::vector<Vertex> Vertices;
};

#endif // NEWTRIANGLE_H
