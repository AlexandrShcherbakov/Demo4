#ifndef DIRECTIONALLIGHTSOURCE_H
#define DIRECTIONALLIGHTSOURCE_H

#ifndef UTILITY_H_INCLUDED
#include "GL\ViewPoint.h"
#endif // UTILITY_H_INCLUDED

namespace GL {

class DirectionalLightSource : public ViewPoint
{
    public:
        virtual VM::mat4 getMatrix() const;

        virtual ~DirectionalLightSource() {}
    protected:
        float InnerRadius, OutterRadius;
    private:
};

}

#endif // DIRECTIONALLIGHTSOURCE_H
