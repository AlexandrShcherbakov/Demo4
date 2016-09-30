#ifndef CUBEWITHTRIANGLES_H
#define CUBEWITHTRIANGLES_H

#include <vector>

#include "Cube.h"
#include "Triangle.h"


class CubeWithTriangles : public Cube
{
	public:
		CubeWithTriangles(const VM::vec4& minPoint, const VM::vec4& maxPoint);

        inline void AddTriangle(const Triangle& triangle) {
            Triangles.push_back(triangle);
        }
        inline void AddTriangles(
			const std::vector<Triangle>::iterator& begin,
			const std::vector<Triangle>::iterator& end) {
            Triangles.insert(Triangles.end(), begin, end);
        }
        inline bool IsEmpty() const {
        	return Triangles.empty();
        }
	protected:
	private:
        std::vector<Triangle> Triangles;
};

#endif // CUBEWITHTRIANGLES_H
