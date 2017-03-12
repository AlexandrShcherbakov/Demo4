#include "Framebuffer.h"

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

void Framebuffer::AttachTexture(const Texture<GL_TEXTURE_2D, GL_R32F>& texture, const GLuint target) {
    if (Width == -1) {
        Width = texture.GetWidth();
        Height = texture.GetHeight();
    }
    if (Width != texture.GetWidth() || Height != texture.GetHeight()) {
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

    glBindTexture(GL_TEXTURE_2D, texture.GetID());                               CHECK_GL_ERRORS;
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, target, GL_TEXTURE_2D, texture.GetID(), 0); CHECK_GL_ERRORS;
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

}
