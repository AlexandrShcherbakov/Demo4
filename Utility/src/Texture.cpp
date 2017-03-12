#include "Texture.h"

namespace GL {

std::shared_ptr<GLuint> TextureIDInitialization(const int IMG_FORMAT) {
    std::shared_ptr<GLuint> ID(
        new GLuint,
        [](GLuint* ID){glDeleteTextures(1, ID);}
    );
    glGenTextures(1, ID.get());                                                  CHECK_GL_ERRORS;
    glBindTexture(IMG_FORMAT, *ID);                                              CHECK_GL_ERRORS;
    glTexParameteri(IMG_FORMAT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);               CHECK_GL_ERRORS;
    glTexParameteri(IMG_FORMAT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);               CHECK_GL_ERRORS;
    glTexParameteri(IMG_FORMAT, GL_TEXTURE_WRAP_S, GL_REPEAT);                   CHECK_GL_ERRORS;
    glTexParameteri(IMG_FORMAT, GL_TEXTURE_WRAP_T, GL_REPEAT);                   CHECK_GL_ERRORS;
    return ID;
}

}
