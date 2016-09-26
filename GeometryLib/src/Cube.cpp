#include "Cube.h"

using namespace VM;

Cube::Cube()
{
	MinPoint = vec4(0, 0, 0, 1);
	MaxPoint = vec4(1, 1, 1, 1);
}

Cube::Cube(const VM::vec4& minPoint, const VM::vec4& maxPoint) {
    MinPoint = minPoint;
    MaxPoint = maxPoint;
}

bool Cube::IntersectsWithVolume(const Volume& volume) {
    return volume.IntersectsWithCube(MinPoint, MaxPoint);
}
