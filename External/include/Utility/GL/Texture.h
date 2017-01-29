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
    	///Constructors
        Texture();
        Texture(const uint width, const uint height);

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
        void SetData(const void* data, const GLuint type, const GLuint internalFormat, const GLuint format, const GLuint dataType) const;
        void SetSlot(const uint slot) {
            Slot = slot;
        }

        ///Other functions
        void LoadFromFile(const std::string& filename) const;
        void BindToShader(ShaderProgram& prog, const std::string& uniformName) const;
        void Bind() const {
            glBindTexture(GL_TEXTURE_2D, *ID);                                   CHECK_GL_ERRORS;
        }
        void BindForComputing(const GLuint location, const GLuint mode=GL_READ_WRITE, const GLuint format=GL_RGBA32F);
        void Unbind() const {
            glBindTexture(GL_TEXTURE_2D, 0);                                     CHECK_GL_ERRORS;
        }

    protected:
        int Width, Height;
        int Slot;
    	std::shared_ptr<GLuint> ID;
};

}

#endif // GL_TEXTURE_H
