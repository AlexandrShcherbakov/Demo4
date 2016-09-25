#include "SpotLightSource.h"

namespace GL {

using namespace VM;

SpotLightSource::SpotLightSource(
       const vec3& position,
	   const vec3& direction,
	   const vec3& color,
	   const float innerCone,
	   const float outterCone
    ) {
       this->position = position;
       this->direction = direction;
       this->color = color;
       this->innerCone = innerCone;
       this->angle = outterCone;
	}

mat4 SpotLightSource::getMatrix() {
    vec3 z = normalize(direction);
    vec3 up;
    if (fabs(z.x) > VEC_EPS || fabs(z.y) > VEC_EPS) {
        up = vec3(z.y, -z.x, 0);
    } else {
		up = vec3(1, 1, 0);
    }
    vec3 x = normalize(cross(up, z));
    vec3 y = normalize(cross(z, x));
    z *= -1;
    mat4 Minv(1);
    mat4 Tr(1);
    Minv[0] = vec4(x, 0);
    Minv[1] = vec4(y, 0);
    Minv[2] = vec4(z, 0);
    Tr[3] = vec4(-position, 1);
    Tr = transpose(Tr);
    Minv = transpose(Minv);
	Minv = Minv.unmatrixN3();

    float f = 1 / tan(angle * 2);
    float zfar = 50.0f;
    float znear = 0.05f;
    float A = (zfar + znear) / (znear - zfar);
    float B = (2 * zfar * znear) / (znear - zfar);

    mat4 projection(0.0f);
    projection[0][0] = f;
    projection[1][1] = f;
    projection[2][2] = A;
    projection[2][3] = B;
    projection[3][2] = -1;
	return projection * Minv * Tr;
}

}
