#ifndef PATCH_H
#define PATCH_H

#include <array>

#include "Utility.h"

class Patch
{
	public:
		Patch();

		inline VM::vec4 GetNormal() const {
            return Normal;
		}

        inline std::array<VM::vec4, 4>::const_iterator PointsBegin() const {
            return Points.begin();
        }
        inline std::array<VM::vec4, 4>::const_iterator PointsEnd() const {
            return Points.end();
        }

        inline VM::vec4 Center() const {
            return (Points[0] + Points[2]) / 2;
        }

        inline VM::vec4 PointByCoords(const VM::vec2& coord) const {
            return Points[0] + (Points[1] - Points[0]) * coord.x + (Points[3] - Points[0]) * coord.y;
        }

        inline float Side() const {
            return VM::length(Points[0] - Points[1]);
        }

		bool Intersect(const VM::vec4& start, const VM::vec4& end) const;

		inline float GetSquare() const {
            return sqr(VM::length(Points[1] - Points[0]));
		}

        bool operator==(const Patch& p) const;

        std::array<VM::vec4, 4> Points;
		VM::vec4 Normal;
		VM::vec4 Color;
        uint Index;

	protected:
	private:
};

#endif // PATCH_H
