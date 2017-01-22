#ifndef VECTOR4_H_INCLUDED
#define VECTOR4_H_INCLUDED

#include <iostream>

#ifndef UTILITY_H_INCLUDED
#include "Overall.h"
#include "VM\vector3.h"
#endif // UTILITY_H_INCLUDED

namespace VM {

template<typename T>
class vector4 {
public:
    const static int Components = 4;

	///Coordinates
    T x, y, z, w;

    ///Constructors
    vector4(): x(T()), y(T()), z(T()), w(T()) {}
    vector4(const T x, const T y, const T z, const T w): x(x), y(y), z(z), w(w) {}
    vector4(const T val): x(val), y(val), z(val), w(val) {}
    vector4(const T* coords): x(coords[0]), y(coords[1]), z(coords[2]), w(coords[3]) {}
    vector4(const vector3<T>& v, const T w): x(v.x), y(v.y), z(v.z), w(w) {}

    ///Operators
    inline friend vector4 operator+(const vector4 &v, const vector4 &w) {
        return vector4(v.x + w.x, v.y + w.y, v.z + w.z, v.w + w.w);
    }

    inline friend vector4 operator-(const vector4 &v, const vector4 &w) {
        return vector4(v.x - w.x, v.y - w.y, v.z - w.z, v.w - w.w);
    }

    inline friend vector4 operator*(const vector4 &v, const vector4 &w) {
        return vector4(v.x * w.x, v.y * w.y, v.z * w.z, v.w * w.w);
    }

    inline friend vector4 operator/(const vector4 &v, const vector4 &w) {
        return vector4(v.x / w.x, v.y / w.y, v.z / w.z, v.w / w.w);
    }

    vector4 operator-() const {
        return vector4(-x, -y, -z, -w);
    }

    inline friend vector4 operator+=(vector4 &v, const vector4 &w) {
        return v = v + w;
    }

    inline friend vector4 operator-=(vector4 &v, const vector4 &w) {
        return v = v - w;
    }

    inline friend vector4 operator*=(vector4 &v, const vector4 &w) {
        return v = v * w;
    }

    inline friend vector4 operator/=(vector4 &v, const vector4 &w) {
        return v = v / w;
    }

    const T& operator[](const uint index) const {
        if (index == 0) return x;
        if (index == 1) return y;
        if (index == 2) return z;
        if (index == 3) return w;
        throw "Too big index for vector4";
    }

    T& operator[](const uint index) {
        return const_cast<T&>(
            static_cast<const vector4<T>& >(*this)[index]
        );
    }

    vector3<T> xyz() const {
        return vector3<T>(x, y, z);
    }
};

template<typename T>
bool operator==(const vector4<T> &v, const vector4<T> &w) {
    return v.x == w.x
        && v.y == w.y
        && v.z == w.z
        && v.w == w.w;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const vector4<T>& v) {
    return out << "(" << v.x << "; " << v.y << "; " << v.z << "; " << v.w << ")";
}

template<typename T>
T min(const vector4<T>& v) {
    return std::min(std::min(v.x, v.y), std::min(v.z, v.w));
}

template<typename T>
T max(const vector4<T>& v) {
    return std::max(std::max(v.x, v.y), std::max(v.z, v.w));
}

template<typename T>
vector4<T> min(const vector4<T>& v, const vector4<T>& w) {
    return vector4<T>(
        std::min(v.x, w.x),
        std::min(v.y, w.y),
        std::min(v.z, w.z),
        std::min(v.w, w.w)
    );
}

template<typename T>
vector4<T> max(const vector4<T>& v, const vector4<T>& w) {
    return vector4<T>(
        std::max(v.x, w.x),
        std::max(v.y, w.y),
        std::max(v.z, w.z),
        std::max(v.w, w.w)
    );
}

///Dot production
template<typename T>
T dot(const vector4<T>& v, const vector4<T>& w) {
    return v.x * w.x + v.y * w.y + v.z * w.z + v.w * w.w;
}

///Cross production
template<typename T>
vector3<T> cross(const vector4<T> &v, const vector4<T> &w) {
    return vector3<T>(
        v.y * w.z - v.z * w.y,
        v.z * w.x - v.x * w.z,
        v.x * w.y - v.y * w.x
    );
}


typedef vector4<float> vec4;
typedef vector4<uint> uvec4;
typedef vector4<short> i16vec4;

}

#endif // VECTOR4_H_INCLUDED
