#include "VM\vec4.h"

namespace VM {

bool operator==(const vec4& v, const vec4& w) {
    return std::abs(v.x - w.x) < VEC_EPS
        && std::abs(v.y - w.y) < VEC_EPS
        && std::abs(v.z - w.z) < VEC_EPS
        && std::abs(v.w - w.w) < VEC_EPS;
}

bool operator!=(const vec4& v, const vec4& w) {
    return !(v == w);
}

vec4 normalize(const vec4& v) {
    float len = length(v);
    if (len < VEC_EPS) throw "Null-length vector normalization.";
    return v / len;
}

float length(const vec4 &v) {
    return sqrt(dot(v, v));
}

}
