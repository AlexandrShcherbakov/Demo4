#include "VM\vec2.h"

namespace VM {

bool operator==(const vec2& v, const vec2& w) {
    return std::abs(v.x - w.x) < VEC_EPS
        && std::abs(v.y - w.y) < VEC_EPS;
}

bool operator!=(const vec2& v, const vec2& w) {
    return !(v == w);
}

vec2 normalize(const vec2& v) {
    float len = length(v);
    if (len < VEC_EPS) throw "Null-length vector normalization.";
    return v / len;
}

float length(const vec2 &v) {
    return sqrt(dot(v, v));
}

}
