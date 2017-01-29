#include "Texture.h"

namespace GL {

std::shared_ptr<GLuint> TextureIDInitialization() {
    std::shared_ptr<GLuint> ID(
        new GLuint,
        [](GLuint* ID){glDeleteTextures(1, ID);}
    );
    glGenTextures(1, ID.get());                                                  CHECK_GL_ERRORS;
    glBindTexture(GL_TEXTURE_2D, *ID);                                           CHECK_GL_ERRORS;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);            CHECK_GL_ERRORS;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);            CHECK_GL_ERRORS;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);                CHECK_GL_ERRORS;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);                CHECK_GL_ERRORS;
    return ID;

}

Texture::Texture():
    Width(0),
    Height(0),
    Slot(0),
    ID(TextureIDInitialization())
{}

Texture::Texture(const uint width, const uint height):
    Width(width),
    Height(height),
    Slot(0),
    ID(TextureIDInitialization())
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, Width, Height, 0, GL_RGBA, GL_FLOAT, NULL); CHECK_GL_ERRORS;
}

void Texture::SetImage(const Image& img) const {
    std::vector<char> vec_data = img.getData();
    char * data = vec_data.data();
	Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); CHECK_GL_ERRORS;
    glGenerateMipmap(GL_TEXTURE_2D);                                             CHECK_GL_ERRORS;
}

void Texture::LoadFromFile(const std::string& filename) const {
	auto img = Image(filename);
	img.UndoGamma();
    SetImage(img);
}

void Texture::BindToShader(ShaderProgram& prog, const std::string& name) const {
    prog.Bind();
	glActiveTexture(GL_TEXTURE0 + Slot);                                         CHECK_GL_ERRORS;
    Bind();
    prog.SetUniform(name, Slot);
    prog.Unbind();
}

void Texture::BindForComputing(const GLuint location, const GLuint mode, const GLuint format) {
    Bind();
    glBindImageTexture(location, *ID, 0, GL_FALSE, 0, mode, format); CHECK_GL_ERRORS;
}

void Texture::SetData(const void* data, const GLuint type, const GLuint internalFormat, const GLuint format, const GLuint dataType) const {
    glTexImage2D(type, 0, internalFormat, Width, Height, 0, format, dataType, data); CHECK_GL_ERRORS;
}

}
