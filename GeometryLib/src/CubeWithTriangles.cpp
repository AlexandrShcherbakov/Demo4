#include "CubeWithTriangles.h"

CubeWithTriangles::CubeWithTriangles(const VM::vec4& minPoint, const VM::vec4& maxPoint) {
    this->MaxPoint = maxPoint;
    this->MinPoint = minPoint;
}
