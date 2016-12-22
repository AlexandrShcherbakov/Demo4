#ifndef VIEWPOINT_H
#define VIEWPOINT_H

#ifndef UTILITY_H_INCLUDED
#include "VM\vec3.h"
#include "VM\mat4.h"
#include <cmath>
#endif // UTILITY_H_INCLUDED

namespace GL {

class ViewPoint
{
	public:
        virtual VM::mat4 getMatrix() = 0;
        VM::vec3 position;
        VM::vec3 direction;

        virtual ~ViewPoint() {};
	protected:
	private:
};

VM::mat4 GenRotateMatrix(const VM::vec3& direction, const VM::vec3& up);
VM::mat4 GenTransferMatrix(const VM::vec3& position);
VM::mat4 GenPerspectiveMatrix(
    const float angle, const float znear,
    const float zfar, const float screenRatio);
VM::mat4 GenScaleMatrix(const float xScale, const float yScale);

}

#endif // VIEWPOINT_H
