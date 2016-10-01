#ifndef PATCH_H
#define PATCH_H

#include <array>

#include "Utility.h"

class Patch
{
	public:
		Patch();
	protected:
	private:
		std::array<VM::vec4, 4> Points;
		VM::vec4 Normal;
		VM::vec4 Color;
};

#endif // PATCH_H
