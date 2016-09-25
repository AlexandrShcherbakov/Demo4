#ifndef POLYGON_H
#define POLYGON_H

#include <vector>

#include "Utility.h"

class TexturedPolygon
{
	public:
		inline void setImage(GL::Image * img) {image = img;}
		inline void setColor(const VM::vec4& color) {this->color = color;}
		inline void setMaterialNumber(const uint mat) {materialNum = mat;}
        inline void addPoint(
			const VM::vec4& point,
			const VM::vec4& normal,
			const VM::vec2& tex_coord) {
            points.push_back(point);
            normals.push_back(normal);
            tex_coords.push_back(tex_coord);
		}

        inline std::vector<VM::vec4> getPoints() const {return points;}
        inline std::vector<VM::vec4> getNormals() const {return normals;}
        inline std::vector<VM::vec2> getTexCoords() const {return tex_coords;}
        inline GL::Image * getImage() const {return image;}
        inline VM::vec4 getColor() const {return color;}
        inline uint getMaterialNumber() const {return materialNum;}
        inline uint getSize() const {return points.size();}
        float getSquare() const;

        bool operator==(const TexturedPolygon &poly) const;
        VM::vec4 center() const;
        VM::vec4 pointByCoords(const VM::vec2& coords) const;
        VM::vec4 normal() const;
        bool intersect(const VM::vec4& start, const VM::vec4& end) const;
        void setIndex(const uint index) {this->index = index;}
        uint getIndex() const {return index;}
        void addRelation(const uint index, const float weight) {
            relation.push_back(index);
            rel_weight.push_back(weight);
        }
        std::vector<uint> getRelations() const {return relation;}
        std::vector<float> getWeights() const {return rel_weight;}
	protected:
	private:
		std::vector<VM::vec4> points;
		std::vector<VM::vec4> normals;
		std::vector<VM::vec2> tex_coords;
		GL::Image * image;
        VM::vec4 color;
        uint materialNum;
        uint index;
        std::vector<uint> relation;
        std::vector<float> rel_weight;
};

#endif // POLYGON_H
