#ifndef PERSPECTIVEVIEWPOINT_H
#define PERSPECTIVEVIEWPOINT_H

#ifndef UTILITY_H_INCLUDED
#include "GL\ViewPoint.h"
#endif // UTILITY_H_INCLUDED

namespace GL {

class PerspectiveViewPoint : public ViewPoint
{
    public:
        float angle;
        virtual ~PerspectiveViewPoint() {};
    protected:
    private:
};

}

#endif // PERSPECTIVEVIEWPOINT_H
