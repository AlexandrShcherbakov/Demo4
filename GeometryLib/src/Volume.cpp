#include "Volume.h"

using namespace VM;

Tube::Tube() {
    Begin = vec4(0, 0, 0, 0);
    End = vec4(1, 0, 0, 0);
    Radius = 1;
    UpdateDirection();
}

Tube::Tube(const vec4& begin, const vec4& end, const float radius) {
    Begin = begin;
    End = end;
    Radius = radius;
    UpdateDirection();
}

void Tube::UpdateDirection() {
    Direction = normalize(End - Begin);
}

void Tube::SetBegin(const vec4& point) {
    Begin = point;
    UpdateDirection();
}

void Tube::SetEnd(const vec4& point) {
    End = point;
    UpdateDirection();
}

bool Tube::IncludesPoint(const vec4& point) const {
	vec4 dirToPoint = point - Begin;
    return length(cross(Direction, dirToPoint)) / length(dirToPoint) <= Radius;
}

bool Tube::IntersectsWithCube(
		const vec4& minPoint,
		const vec4& maxPoint) const
{
    return IncludesPoint(minPoint)
		|| IncludesPoint(vec4(minPoint.x, minPoint.y, maxPoint.z, 1))
		|| IncludesPoint(vec4(minPoint.x, maxPoint.y, minPoint.z, 1))
		|| IncludesPoint(vec4(minPoint.x, maxPoint.y, maxPoint.z, 1))
		|| IncludesPoint(vec4(maxPoint.x, minPoint.y, minPoint.z, 1))
		|| IncludesPoint(vec4(maxPoint.x, minPoint.y, maxPoint.z, 1))
		|| IncludesPoint(vec4(maxPoint.x, maxPoint.y, minPoint.z, 1))
		|| IncludesPoint(maxPoint)
		|| (min(minPoint, Begin) == minPoint && max(maxPoint, Begin) == maxPoint)
		|| (min(minPoint, End) == minPoint && max(maxPoint, End) == maxPoint);
}

bool Capsule::IncludesPoint(const vec4& point) const {
    return (Tube::IncludesPoint(point)
		&& dot(Direction, point - Begin) > 0
		&& dot(Direction, point - End) < 0)
		|| length(point - Begin) < Radius
		|| length(point - End) < Radius;
}


bool Sphere::IntersectsWithCube(
		const vec4& minPoint,
		const vec4& maxPoint) const
{
    return Center.x + Radius >= minPoint.x &&
           Center.y + Radius >= minPoint.y &&
           Center.z + Radius >= minPoint.z &&
           Center.x - Radius <= maxPoint.x &&
           Center.y - Radius <= maxPoint.y &&
           Center.z - Radius <= maxPoint.z;
}
