#ifndef VEC2_H
#define VEC2_H

#include <iostream>
#include <cassert>
#include <cmath>

#ifndef UTILITY_H_INCLUDED
#include "Overall.h"
#include "VM/vector2.h"
#endif // UTILITY_H_INCLUDED

namespace VM {

bool operator==(const vec2 &v, const vec2& w);

bool operator!=(const vec2& v, const vec2& w);

float length(const vec2 &v);

vec2 normalize(const vec2& v);

}
#endif // VEC2_H
