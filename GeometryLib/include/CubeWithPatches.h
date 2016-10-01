#ifndef CUBEWITHPATCHES_H
#define CUBEWITHPATCHES_H

#include <array>

#include "Utility.h"

#include "Cube.h"
#include "Patch.h"


class CubeWithPatches : public Cube
{
	public:
		CubeWithPatches();
	protected:
	private:
        std::vector<Patch> Patches;
        std::vector<uint> Indices;

};

#endif // CUBEWITHPATCHES_H
