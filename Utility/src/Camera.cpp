#include "Camera.h"

using namespace VM;

namespace GL {

Camera::Camera()
{
	//ctor
}

Camera::Camera(const vec3& position,
	   const vec3& direction,
	   const vec3& up,
	   const float angle,
	   const float screenRatio,
	   const float znear,
	   const float zfar
    ) {
       this->position = position;
       this->direction = direction;
       this->up = up;
       this->angle = angle;
       this->screenRatio = screenRatio;
       this->znear = znear;
       this->zfar = zfar;
    }

mat4 Camera::getMatrix() const {
    return GenPerspectiveMatrix(angle, znear, zfar, screenRatio)
         * GenRotateMatrix(direction, up)
         * GenTransferMatrix(position);
}

void Camera::rotateLeft(const float angle) {
	direction = normalize(direction * cosf(angle)
				+ normalize(cross(up, direction)) * sinf(angle));
}

void Camera::rotateY(const float angle) {
    mat4 rot(1);
    rot[0][0] = rot[2][2] = cosf(angle);
    rot[0][2] = -sinf(angle);
    rot[2][0] = sinf(angle);
    direction = normalize((rot * vec4(direction, 0)).xyz());
    up = normalize((rot * vec4(up, 0)).xyz());
}

void Camera::rotateTop(const float angle) {
	vec3 left = cross(up, direction);
	direction = normalize(direction * cosf(angle) + up * sinf(angle));
	up = normalize(cross(direction, left));
}

}
