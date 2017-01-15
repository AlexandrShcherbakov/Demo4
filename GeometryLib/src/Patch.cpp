#include "Patch.h"

using namespace VM;

bool InPolygon(const Patch* pl, vec4 p) {
	float sq = 0;
	for (int i = 1; i <= 4; ++i) {
        sq += length(cross(pl->GetPoint(i % 4) - p, pl->GetPoint(i - 1) - p));
	}
	return std::abs(sq / 2 - pl->GetSquare()) < VEC_EPS;
}

bool Patch::Intersect(const vec4& start, const vec4& end) const {
    vec4 aug = end - start;
    float d = -dot(Normal, Points[0]);
    float t = dot(Normal, aug);
    if (std::abs(t) < VEC_EPS) {
		return false;
    }
    t = -(d + dot(Normal, start)) / t;
    if (t < 0 || t > 1) {
        return false;
    }
    return InPolygon(this, start + aug * t);
}

bool Patch::operator==(const Patch& p) const {
    bool flag = true;
    for (uint i = 0; i < Points.size() && flag; ++i)
		flag = Points[i] == p.Points[i];
    return flag;
}
