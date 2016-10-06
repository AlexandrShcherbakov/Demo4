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

        std::array<VM::vec4, 4> Points;
		std::vector<uint> TrianglesIndices;
		std::vector<float> Weights;
		VM::vec4 Normal;
		VM::vec4 Color;
	protected:
	private:
};

#endif // PATCH_H
