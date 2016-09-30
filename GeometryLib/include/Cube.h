#ifndef CUBE_H
#define CUBE_H

#include "Utility.h"

#include <vector>

#include "Triangle.h"
#include "Volume.h"

class Cube
{
	private:
	protected:
		VM::vec4 MinPoint;
		VM::vec4 MaxPoint;
		std::vector<Triangle> PartsInCube(const Triangle& triangle) const;
	public:
		Cube();
		Cube(const VM::vec4& minPoint, const VM::vec4& maxPoint);

		bool IntersectsWithVolume(const Volume& volume);

        inline VM::vec4 GetMinPoint() const {
            return MinPoint;
        }
        inline VM::vec4 GetMaxPoint() const {
            return MaxPoint;
        }

        virtual void AddTriangle(const Triangle& triangle) = 0;
        virtual void AddTriangles(
			const std::vector<Triangle>::iterator& begin,
			const std::vector<Triangle>::iterator& end) = 0;
};

#endif // CUBE_H
