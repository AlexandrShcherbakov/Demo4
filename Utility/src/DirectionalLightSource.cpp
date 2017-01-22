#include "GL\DirectionalLightSource.h"

namespace GL {

using namespace VM;

mat4 DirectionalLightSource::getMatrix() const {
    vec3 up;
    if (fabs(direction.x) > VEC_EPS || fabs(direction.y) > VEC_EPS) {
        up = vec3(direction.y, -direction.x, 0);
    } else {
		up = vec3(1, 1, 0);
    }
    return GenScaleMatrix(OutterRadius * 2.5, OutterRadius * 2.5)
         * GenRotateMatrix(direction, up)
         * GenTransferMatrix(position);
}

}
