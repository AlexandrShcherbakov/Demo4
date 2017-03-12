#ifndef UTILITY_OVERALL_H
#define UTILITY_OVERALL_H

#include <array>
#include <fstream>
#include <streambuf>
#include <string>
#include <vector>

const float VEC_EPS = 1e-9f;
const float M_PI = 3.14159265f;

///Convenient usings for long typenames
using uint = unsigned;

///Convenient small functions
template <typename T>
inline T sqr(T t) {
    return t * t;
}

template <typename T>
inline void Append(std::vector<T>& head, const std::vector<T>& tail) {
    head.insert(head.end(), tail.begin(), tail.end());
}

template <typename T, uint size>
inline void Append(std::vector<T>& head, const std::array<T, size>& tail) {
    head.insert(head.end(), tail.begin(), tail.end());
}

template <typename T>
inline T Interpolate(const T& p1, const T& p2, const float t) {
    return p1 + (p2 - p1) * t;
}

std::string LoadSource(const std::string& filename);

#endif
