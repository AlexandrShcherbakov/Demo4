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
        inline std::vector<VM::vec2> GetTexCoords() const {
        	Unrealized(__FUNCTION__);
        }
        std::vector<uint> GetIndices() const;
        inline std::vector<uint> GetMaterialNumbers() const {
			Unrealized(__FUNCTION__);
        }
        std::vector<VM::vec4> GetAmbientColors() const;

        inline bool IsLeaf() const {
        	return true;
        }
        inline bool IsEmpty() const {
            return Patches.empty();
        }

        void AddTriangle(const Triangle& triangle) {
        	Unrealized(__FUNCTION__);
        }
        virtual void AddTriangles(
			const std::vector<Triangle>::iterator& begin,
			const std::vector<Triangle>::iterator& end)
		{
			Unrealized(__FUNCTION__);
		}

	protected:
	private:
        std::vector<Patch> Patches;
        std::vector<uint> Indices;

};

#endif // CUBEWITHPATCHES_H
