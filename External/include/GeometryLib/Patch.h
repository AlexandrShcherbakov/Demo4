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
	protected:
	private:
		std::array<VM::vec4, 4> Points;
		std::vector<uint> TrianglesIndices;
		VM::vec4 Normal;
		VM::vec4 Color;
};

#endif // PATCH_H
