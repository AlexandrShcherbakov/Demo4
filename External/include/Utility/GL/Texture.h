#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#ifndef UTILITY_H_INCLUDED
#include "GL\Image.h"
#include "GL\ShaderProgram.h"
#endif // UTILITY_H_INCLUDED

#include <map>
#include <set>

namespace GL {

class Texture {
    public:
    	//Constructors and destructor
        Texture();
        Texture(const Texture& tex);
        Texture(const Image& img);
        Texture(const std::string& filename);
        Texture(const uint width, const uint height);
        ~Texture();

		//Getters
        inline GLuint getID() const { return ID; }

		//Setters
		void setImage(const Image& img);
		void setSlot(const uint slot);

        //Tools
        void load(const std::string& filename);
        void bindTexture(ShaderProgram& prog, const std::string& uniformName);

        uint getWidth() const;
    	uint getHeight() const;
    protected:
    	GLuint ID;
    	void setHeight(const uint height);
    	void setWidth(const uint width);
};

}

#endif // GL_TEXTURE_H
