#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <array>
#include <vector>

#include "Utility.h"
#include "Vertex.h"

class Triangle
{
	public:
	    static const uint PointsCount = 3;

	    ///Getters
        VM::vec4 GetAmbientColor() const {
            return AmbientColor;
        }
        uint GetMaterialNumber() const {
        	return MaterialNumber;
        }
        float GetSquare() const {
            return VM::length(VM::cross(Points[1] - Points[0], Points[2] - Points[0])) / 2;
        }
        const GL::Image* GetImagePointer() const {
            return ImagePointer;
        }
        VM::vec4 GetMeanNormal() const;
        std::vector<Vertex> GetVertices() const;

        ///Setters
        void SetAmbientColor(const VM::vec4& color) {
            AmbientColor = color;
        }
		void SetImagePointer(const GL::Image* image) {
			ImagePointer = image;
		}
		void SetMaterialNumber(const uint materialNumber) {
            MaterialNumber = materialNumber;
		}
        void SetPoint(const uint index, const VM::vec4 point, const VM::vec4 normal, const VM::vec2 texCoord) {
            Points[index] = point;
            Normals[index] = normal;
            TexCoords[index] = texCoord;
        }
		void SetPoints(const VM::vec4* points, const VM::vec4* normals, const VM::vec2* texCoords);

        ///Other functions
        std::array<VM::vec4, 3>::const_iterator PointsBegin() const {
        	return Points.begin();
        }
        std::array<VM::vec4, 3>::const_iterator PointsEnd() const {
            return Points.end();
        }
        std::array<VM::vec4, 3>::const_iterator NormalsBegin() const {
        	return Normals.begin();
        }
        std::array<VM::vec4, 3>::const_iterator NormalsEnd() const {
            return Normals.end();
        }
        std::array<VM::vec2, 3>::const_iterator TexCoordsBegin() const {
        	return TexCoords.begin();
        }
        std::array<VM::vec2, 3>::const_iterator TexCoordsEnd() const {
            return TexCoords.end();
        }
        void InheritParametersFrom(const Triangle& parent);

	protected:
	private:
	    std::array<VM::vec4, PointsCount> Points;
		std::array<VM::vec4, PointsCount> Normals;
		std::array<VM::vec2, PointsCount> TexCoords;
        VM::vec4 AmbientColor;
        const GL::Image * ImagePointer;
        uint MaterialNumber;
};

#endif // TRIANGLE_H
