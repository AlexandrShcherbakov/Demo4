#include "Volume.h"

using namespace VM;

Tube::Tube() {
    Begin = vec4(0, 0, 0, 0);
    End = vec4(1, 0, 0, 0);
    Radius = 1;
    UpdateDirection();
}

Tube::Tube(const VM::vec4& begin, const VM::vec4& end, const float radius) {
    Begin = begin;
    End = end;
    Radius = radius;
    UpdateDirection();
}

void Tube::UpdateDirection() {
    Direction = normalize(End - Begin);
}

void Tube::SetBegin(const VM::vec4& point) {
    Begin = point;
    UpdateDirection();
}

void Tube::SetEnd(const VM::vec4& point) {
    End = point;
    UpdateDirection();
}

bool Tube::IncludesPoint(const VM::vec4& point) const {
	vec4 dirToPoint = point - Begin;
    return length(cross(Direction, dirToPoint)) / length(dirToPoint) <= Radius;
}
