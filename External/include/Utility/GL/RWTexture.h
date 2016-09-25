#ifndef RWTEXTURE_H
#define RWTEXTURE_H

#ifndef UTILITY_H_INCLUDED
#include "GL\Texture.h"
#endif // UTILITY_H_INCLUDED

namespace GL {

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
