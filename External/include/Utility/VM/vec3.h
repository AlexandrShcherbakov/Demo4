#ifndef VEC3_H
#define VEC3_H

#include <algorithm>
#include <iostream>

#ifndef UTILITY_H_INCLUDED
#include "Overall.h"
//#include "VM\vector3.h"
#endif // UTILITY_H_INCLUDED

namespace VM {

class vec3 {
public:
	///Coordinates
    float x, y, z;

    ///Constructors
    vec3(): x(0), y(0), z(0) {}
    vec3(const float x, const float y, const float z): x(x), y(y), z(z) {}
    vec3(const float x): x(x), y(x), z(x) {}
    vec3(const float* coords): x(coords[0]), y(coords[1]), z(coords[2]) {}

    ///Operators
    inline friend vec3 operator+(const vec3& v, const vec3& w) {
        return vec3(v.x + w.x, v.y + w.y, v.z + w.z);
    }
    inline friend vec3 operator-(const vec3& v, const vec3& w) {
        return vec3(v.x - w.x, v.y - w.y, v.z - w.z);
    }
    inline friend vec3 operator*(const vec3& v, const vec3& w) {
        return vec3(v.x * w.x, v.y * w.y, v.z * w.z);
    }
    inline friend vec3 operator/(const vec3& v, const vec3& w) {
        return vec3(v.x / w.x, v.y / w.y, v.z / w.z);
    }
    vec3 operator-() const {
        return vec3(-x, -y, -z);
    }
    inline friend vec3 operator+=(vec3& v, const vec3& w) {
        return v = v + w;
    }
    inline friend vec3 operator-=(vec3& v, const vec3& w) {
        return v = v - w;
    }
    inline friend vec3 operator*=(vec3& v, const vec3& w) {
        return v = v * w;
    }
    inline friend vec3 operator/=(vec3& v, const vec3& w) {
        return v = v / w;
    }
    const float& operator[](const uint index) const {
        if (index == 0) return x;
        if (index == 1) return y;
        if (index == 2) return z;
        throw "Too big index for vec3";
    }
    float& operator[](const uint index) {
        return const_cast<float&>(
            static_cast<const vec3&>(*this)[index]
        );
    }
};

bool operator==(const vec3 &v, const vec3& w);

std::ostream& operator<<(std::ostream& out, const vec3& v);

float min(const vec3& v);

float max(const vec3& v);

vec3 min(const vec3& v, const vec3& w);

vec3 max(const vec3& v, const vec3& w);

///Length of vector for similarity to openGL syntax
float length(const vec3 &v);

vec3 normalize(const vec3 &v);

///Dot production
float dot(const vec3 &v, const vec3 &w);

///Cross production
vec3 cross(const vec3 &v, const vec3 &w);

///Cosine between two vectors
float cos(const vec3 &v, const vec3 &w);


}
#endif // VEC3_H
