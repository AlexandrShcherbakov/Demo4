#ifndef CAMERA_H
#define CAMERA_H

#ifndef UTILITY_H_INCLUDED
#include "VM\vec3.h"
#include "VM\mat4.h"
#include "GL\ViewPoint.h"
#endif // UTILITY_H_INCLUDED

#include <cmath>

namespace GL {

class Camera : public ViewPoint
{
	public:
		Camera();
		Camera(const VM::vec3& position,
			   const VM::vec3& direction,
			   const VM::vec3& up,
			   const float angle,
			   const float screenRatio,
			   const float znear,
			   const float zfar);

		virtual VM::mat4 getMatrix();

		void rotateLeft(const float angle=0.05f);
		void rotateTop(const float angle=0.05f);
		void rotateY(const float angle=0.05f);
        inline void goForward(const float step=0.01f) {position += -direction * step;}
        inline void goBack(const float step=0.01f) {position -= -direction * step;}
        inline void rotateRight(const float angle=0.05f) {rotateLeft(-angle);}

		VM::vec3 up;
        float screenRatio;
        float znear;
        float zfar;
	protected:
	private:
};

}

#endif // CAMERA_H
