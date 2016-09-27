#ifndef CUBE_H
#define CUBE_H

#include "Utility.h"

#include "Volume.h"

class Cube
{
    VM::vec4 MinPoint;
    VM::vec4 MaxPoint;
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

};

#endif // CUBE_H
