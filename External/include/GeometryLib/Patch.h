#ifndef PATCH_H
#define PATCH_H

#include <array>

#include "Utility.h"

class Patch
{
	public:
	    ///Getters
	    uint GetPointsSize() const {
            return Points.size();
	    }
	    VM::vec4 GetPoint(const uint index) const {
            return Points[index];
	    }
	    std::array<VM::vec4, 4> GetPoints() const {
            return Points;
	    }
	    VM::vec4 GetColor() const {
            return Color;
		}
	    VM::vec4 GetNormal() const {
            return Normal;
		}
		uint GetIndex() const {
            return Index;
		}
		float GetSquare() const {
            return sqr(VM::length(Points[1] - Points[0]));
		}
		float GetSide() const {
            return VM::length(Points[0] - Points[1]);
        }
        VM::vec4 GetCenter() const {
            return (Points[0] + Points[2]) / 2;
        }

        ///Setters
        void SetPoint(const uint index, const VM::vec4& point) {
            Points[index] = point;
        }
        void SetColor(const VM::vec4& color) {
            Color = color;
        }
        void SetNormal(const VM::vec4& normal) {
            Normal = normal;
        }
        void SetIndex(const uint index) {
            Index = index;
        }

        ///Other functions
        VM::vec4 PointByCoords(const VM::vec2& coord) const {
            return Points[0] + (Points[1] - Points[0]) * coord.x + (Points[3] - Points[0]) * coord.y;
        }
		bool Intersect(const VM::vec4& start, const VM::vec4& end) const;

	protected:
	private:
	    std::array<VM::vec4, 4> Points;
		VM::vec4 Normal;
		VM::vec4 Color;
        uint Index;
};

#endif // PATCH_H
