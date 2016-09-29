#ifndef CUBEWITHTRIANGLES_H
#define CUBEWITHTRIANGLES_H

#include <vector>

#include "Cube.h"
#include "Triangle.h"


class CubeWithTriangles : public Cube
{
	public:
		CubeWithTriangles();

        inline void AddTriangle(const Triangle& triangle) {
            Triangles.push_back(triangle);
        }
        inline void AddTriangles(const Triangle* begin, const Triangle* end) {
            Triangles.insert(Triangles.end(), begin, end);
        }
	protected:
	private:
        std::vector<Triangle> Triangles;
};

#endif // CUBEWITHTRIANGLES_H
