#ifndef VEC4_H
#define VEC4_H

#ifndef UTILITY_H_INCLUDED
#include "VM\vec3.h"
#endif // UTILITY_H_INCLUDED

namespace VM {

class vec4 {
public:
	///Coordinates
    float x, y, z, w;

    ///Constructors
    vec4(): x(0.0f), y(0.0f), z(0.0f), w(0.0f) {};

    vec4(const float value): x(value), y(value), z(value), w(value) {};

    vec4(const float x, const float y, const float z, const float w):
        x(x), y(y), z(z), w(w) {};

    vec4(const float* coords):
        x(coords[0]), y(coords[1]), z(coords[2]), w(coords[3]) {};

    vec4(const vec3 &v, const float w):
        x(v.x), y(v.y), z(v.z), w(w) {};

	///Operators

    float& operator[](const uint index);
    const float& operator[](const uint index) const;
    bool operator==(const vec4 &v) const;
    bool operator!=(const vec4 &v) const;

    friend std::ostream& operator<<(std::ostream& os, const vec4& v);

    vec3 xyz() const;
};

inline vec4 operator+(const vec4 &v, const vec4 &w) {
    return vec4(v.x + w.x, v.y + w.y, v.z + w.z, v.w + w.w);
}

inline vec4 operator-(const vec4 &v, const vec4 &w) {
    return vec4(v.x - w.x, v.y - w.y, v.z - w.z, v.w - w.w);
}

inline vec4 operator*(const vec4 &v, const vec4 &w) {
    return vec4(v.x * w.x, v.y * w.y, v.z * w.z, v.w * w.w);
}

inline vec4 operator/(const vec4 &v, const vec4 &w) {
    return vec4(v.x / w.x, v.y / w.y, v.z / w.z, v.w / w.w);
}

inline vec4 operator-(const vec4& v) {
    return vec4(-v.x, -v.y, -v.z, -v.w);
}

inline vec4 operator+=(vec4 &v, const vec4& w) {
    return v = v + w;
}

inline vec4 operator-=(vec4 &v, const vec4& w) {
    return v = v - w;
}

inline vec4 operator*=(vec4 &v, const vec4& w) {
    return v = v * w;
}

inline vec4 operator/=(vec4 &v, const vec4& w) {
    return v = v / w;
}

vec4 min(const vec4& v1, const vec4& v2);

vec4 max(const vec4& v1, const vec4& v2);

float min(const vec4& v);

float max(const vec4& v);

///Length of vector for similarity to openGL syntax
float length(const vec4 &v);

vec4 normalize(const vec4& v);

///Dot production
float dot(const vec4 &v, const vec4 &w);

vec3 cross(const vec4 &v, const vec4 &w);

}
#endif // VEC4_H
