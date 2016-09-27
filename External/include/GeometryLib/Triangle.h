#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <array>
#include <vector>

#include "Utility.h"

#include "Cube.h"


class Triangle
{
	public:
		Triangle();

        inline void SetAmbientColor(const VM::vec4& color) {
            AmbientColor = color;
        }
		inline void SetImagePointer(const GL::Image* image) {
			ImagePointer = image;
		}
		void SetPoints(const VM::vec4* points, const VM::vec4* normals, const VM::vec2* texCoords);

        inline std::array<VM::vec4, 3>::const_iterator PointsBegin() const {
        	return Points.begin();
        }

        inline std::array<VM::vec4, 3>::const_iterator PointsEnd() const {
            return Points.end();
        }

        inline std::array<VM::vec4, 3>::const_iterator NormalsBegin() const {
        	return Normals.begin();
        }

        inline std::array<VM::vec4, 3>::const_iterator NormalsEnd() const {
            return Normals.end();
        }

        inline std::array<VM::vec2, 3>::const_iterator TexCoordsBegin() const {
        	return TexCoords.begin();
        }

        inline std::array<VM::vec2, 3>::const_iterator TexCoordsEnd() const {
            return TexCoords.end();
        }

        std::vector<Triangle> PartsInCube(const Cube& cube) const;

	protected:
	private:
		std::array<VM::vec4, 3> Points;
		std::array<VM::vec4, 3> Normals;
		std::array<VM::vec2, 3> TexCoords;
        VM::vec4 AmbientColor;
        const GL::Image * ImagePointer;
};

#endif // TRIANGLE_H
