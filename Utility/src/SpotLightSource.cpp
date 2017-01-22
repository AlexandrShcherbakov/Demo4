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

mat4 SpotLightSource::getMatrix() const {
    vec3 up;
    if (fabs(direction.x) > VEC_EPS || fabs(direction.y) > VEC_EPS) {
        up = vec3(direction.y, -direction.x, 0);
    } else {
		up = vec3(1, 1, 0);
    }

    return GenPerspectiveMatrix(angle * 4, 0.01f, 10000.0f, 1.0f)
         * GenRotateMatrix(-direction, up)
         * GenTransferMatrix(position);
}

}
