#include "VM\vec3.h"

#include <algorithm>

namespace VM {
///Operators
bool vec3::operator==(const vec3 &v) const {
    return std::abs(x - v.x) < VEC_EPS && std::abs(y - v.y) < VEC_EPS && std::abs(z - v.z) < VEC_EPS;
}

std::ostream& operator<<(std::ostream& os, const vec3& v)
{
    os << "(" << v.x << "; " << v.y << "; " << v.z << ")";
    return os;
}

float min(const vec3& v) {
    return std::min(std::min(v.x, v.y), v.z);
}

float max(const vec3& v) {
	return std::max(std::max(v.x, v.y), v.z);
}

vec3 min(const vec3& v, const vec3& w) {
    return vec3(std::min(v.x, w.x), std::min(v.y, w.y), std::min(v.z, w.z));
}

vec3 max(const vec3& v, const vec3& w) {
    return vec3(std::max(v.x, w.x), std::max(v.y, w.y), std::max(v.z, w.z));
}

vec3 normalize(const vec3& v) {
    float len = length(v);
    if (len == 0.0f) throw "Null-length vector normalization.";
    return v / len;
}

float length(const vec3 &v) {
    return sqrt(dot(v, v));
}

///Dot production
float dot(const vec3 &v, const vec3 &w) {
    return v.x * w.x + v.y * w.y + v.z * w.z;
}

///Cross production
vec3 cross(const vec3 &v, const vec3 &w) {
    return vec3(v[1] * w[2] - v[2] * w[1],
                v[2] * w[0] - v[0] * w[2],
                v[0] * w[1] - v[1] * w[0]);
}

///Cosine between two vectors
float cos(const vec3 &v, const vec3 &w) {
    return dot(v, w) / length(v) / length(w);

}

}
