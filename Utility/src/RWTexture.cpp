#include "RWTexture.h"

namespace GL {

Framebuffer::Framebuffer(const int screenWidth, const int screenHeight):
    Width(-1),
    Height(-1),
    ScreenWidth(screenWidth),
    ScreenHeight(screenHeight),
    ID(std::shared_ptr<GLuint>(new GLuint, [](GLuint* ID) {glDeleteFramebuffers(1, ID);}))
{
    glGenFramebuffers(1, ID.get());                                              CHECK_GL_ERRORS
}

void Framebuffer::AttachTexture(const Texture& texture, const GLuint target) {
    if (Width == -1) {
        Width = texture.getWidth();
        Height = texture.getHeight();
    }
    if (Width != texture.getWidth() || Height != texture.getHeight()) {
        throw "Incorrect texture size for framebuffer";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, *ID);                                      CHECK_GL_ERRORS;
    glGenRenderbuffers(1, &Renderbuffer);                                        CHECK_GL_ERRORS;
	glBindRenderbuffer(GL_RENDERBUFFER, Renderbuffer);                           CHECK_GL_ERRORS;
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, Width, Height); CHECK_GL_ERRORS;
	glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        Renderbuffer
    );                                                                           CHECK_GL_ERRORS;

    glBindTexture(GL_TEXTURE_2D, texture.getID());                               CHECK_GL_ERRORS;
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, target, GL_TEXTURE_2D, texture.getID(), 0); CHECK_GL_ERRORS;
}

void Framebuffer::Bind() const {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *ID);                                 CHECK_GL_ERRORS;
    glViewport(0, 0, Width, Height);                                             CHECK_GL_ERRORS;
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);                          CHECK_GL_ERRORS;
}

void Framebuffer::Unbind() const {
    glViewport(0, 0, ScreenWidth, ScreenHeight);                                 CHECK_GL_ERRORS;
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);                                   CHECK_GL_ERRORS;
}

const static GLuint UNDEFINED_TEXTURE_ID = -1;

RWTexture::RWTexture(const uint width, const uint height): Texture(width, height) {
	glGenFramebuffers(1, &framebuffer);                                          CHECK_GL_ERRORS
    //Create texture for shadow map
    /*glBindTexture(GL_TEXTURE_2D, ID);                                            CHECK_GL_ERRORS
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width,
				height, 0, GL_RGBA, GL_FLOAT, NULL);                             CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);            CHECK_GL_ERRORS
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);            CHECK_GL_ERRORS
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);         CHECK_GL_ERRORS
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);         CHECK_GL_ERRORS*/
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);                              CHECK_GL_ERRORS
	glGenRenderbuffers(1, &renderbuffer);                                        CHECK_GL_ERRORS
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);                           CHECK_GL_ERRORS
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height); CHECK_GL_ERRORS
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, renderbuffer);                                          CHECK_GL_ERRORS
    glBindTexture(GL_TEXTURE_2D, ID);                                            CHECK_GL_ERRORS
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							GL_TEXTURE_2D, ID, 0);                               CHECK_GL_ERRORS
}

void RWTexture::bindForDraw() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);                         CHECK_GL_ERRORS
    glViewport(0, 0, getWidth(), getHeight());                                   CHECK_GL_ERRORS
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);                          CHECK_GL_ERRORS
}

void RWTexture::unbind() {
	glViewport(0, 0, 800, 600);                                                  CHECK_GL_ERRORS
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);                                   CHECK_GL_ERRORS
}

}
