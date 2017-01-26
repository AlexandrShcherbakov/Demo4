#ifndef RWTEXTURE_H
#define RWTEXTURE_H

#ifndef UTILITY_H_INCLUDED
#include "GL\Texture.h"
#endif // UTILITY_H_INCLUDED

namespace GL {

class Framebuffer {
	public:
        Framebuffer(const int screenWidth, const int screenHeight);

        void AttachTexture(
            const Texture& texture,
            const GLuint target=GL_COLOR_ATTACHMENT0
        );
        void Bind() const;
        void Unbind() const;

    private:
        int Width, Height;
        int ScreenWidth, ScreenHeight;
        std::shared_ptr<GLuint> ID;
        GLuint Renderbuffer;
};

}

#endif // RWTEXTURE_H
