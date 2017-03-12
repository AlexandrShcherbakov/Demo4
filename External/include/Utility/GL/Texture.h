#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#ifndef UTILITY_H_INCLUDED
#include "GL\Image.h"
#include "GL\ShaderProgram.h"
#endif // UTILITY_H_INCLUDED

#include <map>
#include <set>

namespace GL {

std::shared_ptr<GLuint> TextureIDInitialization(const int IMG_FORMAT);

template<int IMG_FORMAT, int INTERNAL_FORMAT>
class Texture {
    public:
    	///Constructors
        Texture():
            Width(0),
            Height(0),
            ID(TextureIDInitialization(IMG_FORMAT))
        {}
        Texture(const uint width, const uint height, const GLenum type=GL_FLOAT);

        ///Getters
        GLuint GetID() const {
            return *ID;
        }
        int GetWidth() const {
            return Width;
        }
        int GetHeight() const {
            return Height;
        }

        ///Setters
        void SetImage(const Image& img) const;

        ///Other functions
        void LoadFromFile(const std::string& filename) const;
        void Bind() const {
            glBindTexture(GL_TEXTURE_2D, *ID);                                   CHECK_GL_ERRORS;
        }
        void Unbind() const {
            glBindTexture(GL_TEXTURE_2D, 0);                                     CHECK_GL_ERRORS;
        }

    protected:
        int Width, Height;
    	std::shared_ptr<GLuint> ID;
};

template<int IMG_FORMAT, int INTERNAL_FORMAT>
Texture<IMG_FORMAT, INTERNAL_FORMAT>::Texture(const uint width, const uint height, const GLenum type):
    Width(width),
    Height(height),
    ID(TextureIDInitialization(IMG_FORMAT))
{
	glTexImage2D(IMG_FORMAT, 0, INTERNAL_FORMAT, Width, Height, 0, GL_RGBA, type, NULL); CHECK_GL_ERRORS;
}

template<int IMG_FORMAT, int INTERNAL_FORMAT>
void Texture<IMG_FORMAT, INTERNAL_FORMAT>::SetImage(const Image& img) const {
    std::vector<char> vec_data = img.getData();
    char * data = vec_data.data();
	Bind();
    glTexImage2D(IMG_FORMAT, 0, INTERNAL_FORMAT, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); CHECK_GL_ERRORS;
}

template<int IMG_FORMAT, int INTERNAL_FORMAT>
void Texture<IMG_FORMAT, INTERNAL_FORMAT>::LoadFromFile(const std::string& filename) const {
	auto img = Image(filename);
	img.UndoGamma();
    SetImage(img);
}

}

#endif // GL_TEXTURE_H
