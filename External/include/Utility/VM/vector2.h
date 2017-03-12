#ifndef VECTOR2_H_INCLUDED
#define VECTOR2_H_INCLUDED

namespace VM {

template<typename T>
class vector2 {
public:
    const static int Components = 2;

	///Coordinates
    T x, y;

    ///Constructors
    vector2(): x(T()), y(T()) {}
    vector2(const T x, const T y): x(x), y(y) {}
    vector2(const T val): x(val), y(val) {}
    vector2(const T* coords): x(coords[0]), y(coords[1]) {}

    ///Operators
    inline friend const vector2 operator+(const vector2 &v, const vector2 &w) {
        return vector2(v.x + w.x, v.y + w.y);
    }

    inline friend const vector2 operator-(const vector2 &v, const vector2 &w) {
        return vector2(v.x - w.x, v.y - w.y);
    }

    inline friend const vector2 operator*(const vector2 &v, const vector2 &w) {
        return vector2(v.x * w.x, v.y * w.y);
    }

    inline friend const vector2 operator/(const vector2 &v, const vector2 &w) {
        return vector2(v.x / w.x, v.y / w.y);
    }

    const vector2 operator-() const {
        return vector2(-x, -y);
    }

    inline friend vector2 operator+=(vector2 &v, const vector2 &w) {
        return v = v + w;
    }

    inline friend vector2 operator-=(vector2 &v, const vector2 &w) {
        return v = v - w;
    }

    inline friend vector2 operator*=(vector2 &v, const vector2 &w) {
        return v = v * w;
    }

    inline friend vector2 operator/=(vector2 &v, const vector2 &w) {
        return v = v / w;
    }

    const T& operator[](const uint index) const {
        if (index == 0) return x;
        if (index == 1) return y;
        throw "Too big index for vector2";
    }

    T& operator[](const uint index) {
        return const_cast<T&>(
            static_cast<const vector2<T>& >(*this)[index]
        );
    }
};

template<typename T>
inline bool operator==(const vector2<T> &v, const vector2<T> &w) {
    return v.x == w.x
        && v.y == w.y;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const vector2<T>& v) {
    return out << "(" << v.x << "; " << v.y << ")";
}

template<typename T>
const T min(const vector2<T>& v) {
    return std::min(v.x, v.y);
}

template<typename T>
const T max(const vector2<T>& v) {
    return std::max(v.x, v.y);
}

template<typename T>
const vector2<T> min(const vector2<T>& v, const vector2<T>& w) {
    return vector2<T>(std::min(v.x, w.x), std::min(v.y, w.y));
}

template<typename T>
const vector2<T> max(const vector2<T>& v, const vector2<T>& w) {
    return vector2<T>(std::max(v.x, w.x), std::max(v.y, w.y));
}

///Dot production
template<typename T>
const T dot(const vector2<T>& v, const vector2<T>& w) {
    return v.x * w.x + v.y * w.y;
}

using vec2 = vector2<float>;

}

#endif // VECTOR2_H_INCLUDED
