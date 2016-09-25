#include "Buffer.h"

using namespace VM;

namespace GL {

const static GLuint UNDEFINED_BUFFER_ID = -1;

struct BufferInfo {
    uint count;
	GLenum bufferType;
	GLenum elementsType;
	uint size;
	uint components;
};

std::map<uint, BufferInfo> BufferInstanceFactory;

Buffer::Buffer() {
	ID = UNDEFINED_BUFFER_ID;
}

Buffer::Buffer(const Buffer& buf) {
    ID = buf.ID;
    BufferInstanceFactory[ID].count++;
}

Buffer::Buffer(const GLenum elType, const GLenum bufType) {
    glGenBuffers(1, &ID);                                                        CHECK_GL_ERRORS
    BufferInstanceFactory[ID].count = 1;
    BufferInstanceFactory[ID].bufferType = bufType;
    BufferInstanceFactory[ID].elementsType = elType;
}

Buffer::~Buffer() {
    if (ID == UNDEFINED_BUFFER_ID) return;
	BufferInstanceFactory[ID].count--;
	if (!BufferInstanceFactory[ID].count)
		glDeleteBuffers(1, &ID);                                                 CHECK_GL_ERRORS
}

GLenum Buffer::getElementsType() const {
	if (ID == UNDEFINED_BUFFER_ID) return false;
	return BufferInstanceFactory[ID].elementsType;
}

GLenum Buffer::getBufferType() const {
	if (ID == UNDEFINED_BUFFER_ID) return false;
	return BufferInstanceFactory[ID].bufferType;
}

uint Buffer::getSize() const {
	if (ID == UNDEFINED_BUFFER_ID) return false;
	return BufferInstanceFactory[ID].size;
}

uint Buffer::getComponents() const {
	if (ID == UNDEFINED_BUFFER_ID) return false;
    return BufferInstanceFactory[ID].components;
}

void Buffer::setData(const void * data, const uint size) {
	if (ID == UNDEFINED_BUFFER_ID) return;
    glBindBuffer(BufferInstanceFactory[ID].bufferType, ID);                      CHECK_GL_ERRORS
    glBufferData(BufferInstanceFactory[ID].bufferType, size, data, GL_STATIC_DRAW); CHECK_GL_ERRORS
    glBindBuffer(BufferInstanceFactory[ID].bufferType, 0);                       CHECK_GL_ERRORS
}

void Buffer::setData(const std::vector<uint>& data) {
	if (ID == UNDEFINED_BUFFER_ID) return;
    setData(data.data(), sizeof(uint) * data.size());
    BufferInstanceFactory[ID].components = 1;
    BufferInstanceFactory[ID].size = data.size();
}

void Buffer::setData(const std::vector<vec4>& data) {
	if (ID == UNDEFINED_BUFFER_ID) return;
    setData(data.data(), sizeof(vec4) * data.size());
    BufferInstanceFactory[ID].components = 4;
    BufferInstanceFactory[ID].size = data.size();
}

void Buffer::setData(const std::vector<vec2>& data) {
	if (ID == UNDEFINED_BUFFER_ID) return;
    setData(data.data(), sizeof(vec2) * data.size());
    BufferInstanceFactory[ID].components = 2;
    BufferInstanceFactory[ID].size = data.size();
}

}
