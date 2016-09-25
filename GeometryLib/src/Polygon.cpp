#include "Polygon.h"

using namespace VM;

float TexturedPolygon::getSquare() const {
    float square = 0.0f;
    for (uint i = 2; i < points.size(); ++i) {
        square += length(cross(points[i] - points[0], points[i - 1] - points[0]));
    }
    return square / 2.0f;
}

bool TexturedPolygon::operator==(const TexturedPolygon &poly) const {
    if (this->getSize() != poly.getSize()) return false;
    for (uint i = 0; i < poly.getSize(); ++i) {
        if (poly.getPoints()[i] == this->points[0]) {
            bool flag = true;
            for (uint j = 0; j < poly.getSize() && flag; ++j) {
                flag = (poly.getPoints()[(j + i) % poly.getSize()] == this->points[j]);
            }
            return flag;
        }
    }
    return false;
}

vec4 TexturedPolygon::center() const {
    vec4 result(0, 0, 0, 0);
    for (uint i = 0; i < points.size(); ++i)
        result += points[i];
    return result / points.size();
}


vec4 TexturedPolygon::normal() const {
    vec4 normal(0, 0, 0, 0);
    for (auto & norm : normals)
        normal += norm;
	return vec4(normalize(normal.xyz() / normals.size()), 0);
}

vec4 TexturedPolygon::pointByCoords(const vec2& coords) const {
    return vec4((points[0] + (points[1] - points[0]) * coords.x + (points[3] - points[0]) * coords.y).xyz(), 1);
}


bool InPolygon(const TexturedPolygon* pl, vec4 p) {
	float sq = 0;
	for (int i = 1; i <= 4; ++i) {
        sq += length(cross(pl->getPoints()[i % 4] - p, pl->getPoints()[i - 1] - p));
	}
	return std::abs(sq / 2 - pl->getSquare()) < VEC_EPS;
}

bool TexturedPolygon::intersect(const vec4& start, const vec4& end) const {
    vec4 aug = end - start;
    float d = -dot(normal(), points[0]);
    float t = dot(normal(), aug);
    if (std::abs(t) < VEC_EPS) return false;
    t = -(d + dot(normal(), start)) / t;
    if (t < 0 || t > 1) {
        return 0;
    }
    return InPolygon(this, start + aug * t);
}
