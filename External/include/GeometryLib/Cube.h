#ifndef CUBE_H
#define CUBE_H

#include "Utility.h"

#include <string>
#include <vector>
#include <typeinfo>

#include "Triangle.h"
#include "Volume.h"

class Cube
{
	private:
	protected:
		VM::vec4 MinPoint;
		VM::vec4 MaxPoint;
		std::vector<Triangle> PartsInCube(const Triangle& triangle) const;

        inline void Unrealized(const std::string& method_name) const {
            throw "Method " + method_name
             + " didn`t realized in class " + typeid(*this).name();
        }
	public:
		uint Depth;
		std::array<Cube*, 8> * Subnodes;

		Cube();
		Cube(const VM::vec4& minPoint, const VM::vec4& maxPoint);

		bool IntersectsWithVolume(const Volume& volume);

        inline VM::vec4 GetMinPoint() const {
            return MinPoint;
        }
        inline VM::vec4 GetMaxPoint() const {
            return MaxPoint;
        }
        virtual std::vector<VM::vec4> GetPoints() const = 0;
        virtual std::vector<VM::vec4> GetNormals() const = 0;
        virtual std::vector<VM::vec2> GetTexCoords() const = 0;
        virtual std::vector<uint> GetIndices() const = 0;
        virtual std::vector<uint> GetMaterialNumbers() const = 0;
        virtual std::vector<VM::vec4> GetAmbientColors() const = 0;

		virtual void SetIndices(uint& index) = 0;

        virtual bool IsLeaf() const = 0;
        virtual bool IsEmpty() const = 0;

		virtual const Cube* operator[](const VM::uvec3& index) const = 0;

        virtual void AddTriangle(const Triangle& triangle) = 0;
        virtual void AddTriangles(
			const std::vector<Triangle>::iterator& begin,
			const std::vector<Triangle>::iterator& end) = 0;

		virtual void CreateFromTriangles(
			const Cube& octree,
			const VM::uvec3& index,
			const uint side) = 0;
};

#endif // CUBE_H
