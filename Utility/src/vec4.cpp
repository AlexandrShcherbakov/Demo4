#include "VM\vec4.h"

namespace VM {

///Operators

vec4 min(const vec4& v1, const vec4& v2) {
    return vec4(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z), std::min(v1.w, v2.w));
}

vec4 max(const vec4& v1, const vec4& v2) {
    return vec4(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z), std::max(v1.w, v2.w));
}

float min(const vec4& v) {
    return std::min(std::min(v.x, v.y), std::min(v.z, v.w));
}

float max(const vec4& v) {
    return std::max(std::max(v.x, v.y), std::max(v.z, v.w));
}

vec4 normalize(const vec4& v) {
    float len = length(v);
    if (len < VEC_EPS) throw "Null-length vector normalization.";
    return v / len;
}

float length(const vec4 &v) {
    return sqrt(dot(v, v));
}

///Dot production
float dot(const vec4 &v, const vec4 &w) {
    return v.x * w.x + v.y * w.y + v.z * w.z + v.w * w.w;
}

///Cross production
vec3 cross(const vec4 &v, const vec4 &w) {
    return cross(v.xyz(), w.xyz());
}


}
