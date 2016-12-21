#include "ViewPoint.h"

using namespace VM;

namespace GL {

    mat4 GenRotateMatrix(const vec3& direction, const vec3& up) {
        vec3 z = normalize(direction);
        vec3 x = normalize(cross(up, z));
        vec3 y = normalize(cross(z, x));
        mat4 Rotate(1);
        Rotate[0] = vec4(x, 0);
        Rotate[1] = vec4(y, 0);
        Rotate[2] = vec4(z, 0);
        return transpose(Rotate).unmatrixN3();
    }

    mat4 GenTransferMatrix(const vec3& position) {
        mat4 Transfer(1);
        Transfer[3] = vec4(-position, 1);
        return transpose(Transfer);
    }

    mat4 GenPerspectiveMatrix(
        const float angle,
        const float znear,
        const float zfar,
        const float screenRatio
    ) {
        float f = 1 / tan(angle / 2);
        float A = (zfar + znear) / (znear - zfar);
        float B = (2 * zfar * znear) / (znear - zfar);

        mat4 perspective(0.0f);
        perspective[0][0] = f / screenRatio;
        perspective[1][1] = f;
        perspective[2][2] = A;
        perspective[2][3] = B;
        perspective[3][2] = -1;
        return perspective;
    }

}
