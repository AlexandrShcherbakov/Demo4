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

}

#endif // VIEWPOINT_H
