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

class RWTexture : public Texture
{
	public:
		RWTexture(const uint width, const uint height);
        void bindForDraw();
        void unbind();
	protected:
	private:
		uint framebuffer;
		uint renderbuffer;
		uint screenWidth;
		uint screenHeight;
};

}

#endif // RWTEXTURE_H
