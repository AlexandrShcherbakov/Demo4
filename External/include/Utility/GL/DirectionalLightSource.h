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

        ///Getters
        VM::vec3 GetColor() const {
            return Color;
        }

        float GetInnerRadius() const {
            return InnerRadius;
        }

        float GetOutterRadius() const {
            return OutterRadius;
        }

        ///Setters
        void SetInnerRadius(const float innerRadius) {
            InnerRadius = innerRadius;
        }

        void SetOutterRadius(const float outterRadius) {
            OutterRadius = outterRadius;
        }

        void SetColor(const VM::vec3& color) {
            Color = color;
        }

        virtual ~DirectionalLightSource() {}
    protected:
        VM::vec3 Color;
        float InnerRadius, OutterRadius;
    private:
};

}

#endif // DIRECTIONALLIGHTSOURCE_H
