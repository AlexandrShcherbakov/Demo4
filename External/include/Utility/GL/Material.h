#ifndef MATERIAL_H
#define MATERIAL_H

#ifndef UTILITY_H_INCLUDED
#include "GL.h"
#include "GL\Buffer.h"
#include "GL\ShaderProgram.h"
#include "GL\Texture.h"
#endif // UTILITY_H_INCLUDED

namespace GL {

class Material
{
	public:
		Material(const VM::vec4& color=VM::vec4(0, 0, 0, 0), Texture<GL_TEXTURE_2D, GL_RGBA>* tex=nullptr);

		//Setters
        inline void setImage(Texture<GL_TEXTURE_2D, GL_RGBA>* tex) {this->tex = tex;}
        inline void setImage(const Image& img) {
        	this->tex = new Texture<GL_TEXTURE_2D, GL_RGBA>();
        	this->tex->SetImage(img);
        	this->img = img;
		}
        inline void setColor(const VM::vec4& color) {this->color = color;}

		//Tools
		inline bool hasTexture() const {return tex == nullptr;}
        void bindMaterial(
			ShaderProgram& program,
			const std::string& color_name="material_color",
			const std::string& tex_name="material_texture"
		);
	protected:
	private:
		Texture<GL_TEXTURE_2D, GL_RGBA> *tex;
		Image img;
        VM::vec4 color;
};

}

#endif // MATERIAL_H
