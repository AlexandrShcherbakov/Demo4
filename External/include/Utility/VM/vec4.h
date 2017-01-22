#ifndef VEC4_H
#define VEC4_H

#include <algorithm>

#ifndef UTILITY_H_INCLUDED
#include "VM\vector4.h"
#endif // UTILITY_H_INCLUDED

namespace VM {

bool operator==(const vec4 &v, const vec4& w);

bool operator!=(const vec4& v, const vec4& w);

float length(const vec4 &v);

vec4 normalize(const vec4& v);

}
#endif // VEC4_H
