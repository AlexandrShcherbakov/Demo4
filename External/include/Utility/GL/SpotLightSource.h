#ifndef SPOTLIGHTSOURCE_H
#define SPOTLIGHTSOURCE_H

#ifndef UTILITY_H_INCLUDED
#include "GL\PerspectiveViewPoint.h"
#include "VM\vec3.h"
#include "VM\mat4.h"
#include <cmath>
#endif // UTILITY_H_INCLUDED

namespace GL {

class SpotLightSource : public PerspectiveViewPoint
{
	public:
		SpotLightSource(const VM::vec3& position = VM::vec3(0, 0.25, 0),
						const VM::vec3& direction = VM::vec3(0, -1, 0),
						const VM::vec3& color = VM::vec3(1, 1, 1),
						const float innerCone = 30.0f,
						const float outterCone = 60.0f);

        virtual VM::mat4 getMatrix() const;

		VM::vec3 color;
        float innerCone;

        virtual ~SpotLightSource() {}
	protected:
	private:
};

}

#endif // SPOTLIGHTSOURCE_H
