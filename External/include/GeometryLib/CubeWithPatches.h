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

		std::vector<VM::vec4> GetPoints() const;
        std::vector<VM::vec4> GetNormals() const;
        std::vector<VM::vec2> GetTexCoords() const;
        std::vector<uint> GetIndices() const;
        std::vector<uint> GetMaterialNumbers() const;
        std::vector<VM::vec4> GetAmbientColors() const;

        inline bool IsLeaf() const {
        	return true;
        }
        inline bool IsEmpty() const {
            return Patches.empty();
        }

        void AddTriangle(const Triangle& triangle);
        void AddTriangles(
			const std::vector<Triangle>::iterator& begin,
			const std::vector<Triangle>::iterator& end);

	protected:
	private:
        std::vector<Patch> Patches;
        std::vector<uint> Indices;

};

#endif // CUBEWITHPATCHES_H
