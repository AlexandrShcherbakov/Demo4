#include "Texture.h"

namespace GL {

const static GLuint UNDEFINED_TEXTURE_ID = -1;

struct TextureInfo {
    uint count;
	uint width;
	uint height;
	uint externalID;
};

std::map<uint, TextureInfo> TextureInstanceFactory;

Texture::Texture() {
	ID = UNDEFINED_TEXTURE_ID;
}

Texture::Texture(const Texture& tex) {
	ID = tex.ID;
    TextureInstanceFactory[ID].count++;
}

Texture::Texture(const uint width, const uint height) {
	glGenTextures(1, &ID);                                                       CHECK_GL_ERRORS
	glBindTexture(GL_TEXTURE_2D, ID);                                            CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);            CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);            CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);         CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);         CHECK_GL_ERRORS
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RGBA,
          GL_FLOAT, NULL);
    TextureInstanceFactory[ID].count++;
    TextureInstanceFactory[ID].width = width;
    TextureInstanceFactory[ID].height = height;
}

void Texture::setImage(const Image& img) {
	if (ID == UNDEFINED_TEXTURE_ID) return;
    std::vector<char> vec_data = img.getData();
    char * data = vec_data.data();

	glBindTexture(GL_TEXTURE_2D, ID);                                            CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);            CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);            CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);                CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);                CHECK_GL_ERRORS
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA,
          GL_UNSIGNED_BYTE, data);                                               CHECK_GL_ERRORS
    glGenerateMipmap(GL_TEXTURE_2D);                                             CHECK_GL_ERRORS

    TextureInstanceFactory[ID].width = img.width;
    TextureInstanceFactory[ID].height = img.height;
}

void Texture::load(const std::string& filename) {
	if (ID == UNDEFINED_TEXTURE_ID) return;
	auto img = Image(filename);
	img.UndoGamma();
    setImage(img);
}

Texture::Texture(const std::string& filename) {
	glGenTextures(1, &ID);                                                       CHECK_GL_ERRORS
    TextureInstanceFactory[ID].count++;
	load(filename);
}

Texture::Texture(const Image& img) {
	glGenTextures(1, &ID);                                                       CHECK_GL_ERRORS
    TextureInstanceFactory[ID].count++;
	setImage(img);
}

Texture::~Texture() {
	if (ID == UNDEFINED_TEXTURE_ID) return;

	TextureInstanceFactory[ID].count--;
	if (!TextureInstanceFactory[ID].count) {
		glDeleteBuffers(1, &ID);                                                 CHECK_GL_ERRORS
	}
}

void Texture::bindTexture(ShaderProgram& prog, const std::string& uniform) {
    if (ID == UNDEFINED_TEXTURE_ID) return;

    prog.Bind();
	glActiveTexture(GL_TEXTURE0 + TextureInstanceFactory[ID].externalID);        CHECK_GL_ERRORS
    glBindTexture(GL_TEXTURE_2D, ID);                                            CHECK_GL_ERRORS

    prog.SetUniform(uniform, (int)TextureInstanceFactory[ID].externalID);        CHECK_GL_ERRORS
}

uint Texture::getHeight() const {return TextureInstanceFactory[ID].height;}
uint Texture::getWidth() const {return TextureInstanceFactory[ID].width;}
void Texture::setHeight(const uint height) {TextureInstanceFactory[ID].height = height;}
void Texture::setWidth(const uint width) {TextureInstanceFactory[ID].width = width;}
void Texture::setSlot(const uint slot) {TextureInstanceFactory[ID].externalID = slot;}

}
