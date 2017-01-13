#include "VM\vec3.h"

namespace VM {
///Operators
bool operator==(const vec3 &v, const vec3& w) {
    return std::abs(v.x - w.x) < VEC_EPS
        && std::abs(v.y - w.y) < VEC_EPS
        && std::abs(v.z - w.z) < VEC_EPS;
}

vec3 normalize(const vec3& v) {
    float len = length(v);
    if (len == 0.0f) throw "Null-length vector normalization.";
    return v / len;
}

float length(const vec3 &v) {
    return sqrt(dot(v, v));
}

///Cosine between two vectors
float cos(const vec3 &v, const vec3 &w) {
    return dot(v, w) / length(v) / length(w);

}

}
