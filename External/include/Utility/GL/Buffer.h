#ifndef BUFFER_H
#define BUFFER_H

#ifndef UTILITY_H_INCLUDED
#include "GL.h"
#include "VM\vec2.h"
#include "VM\vec4.h"
#endif // UTILITY_H_INCLUDED

#include <map>
#include <memory>
#include <vector>

namespace GL {

template<int BufferType, int GLDataType, typename DataType>
class Buffer
{
	public:
		///Constructors
		Buffer() {
            ID = std::shared_ptr<GLuint>(new GLuint, [](GLuint* ID) {glDeleteBuffers(1, ID);});
            glGenBuffers(1, ID.get());
		}

		///Getters
        int GetSize() const {
            return Size;
        }
        GLuint GetID() const {
            return *ID;
        }
        std::vector<DataType> GetData() const {
            std::vector<DataType> res(Size);
            Bind();
            void * p = glMapBuffer(BufferType, GL_READ_WRITE);                                CHECK_GL_ERRORS;
            memcpy(&res[0], p, Size * sizeof(DataType));
            glUnmapBuffer(BufferType);                                                        CHECK_GL_ERRORS;
            Unbind();
            return res;
        }

		///Setters
		void SetData(const DataType* data, const uint size);
        void SetData(const std::vector<DataType>& data) {
            SetData(data.data(), data.size() * sizeof(DataType));
            Size = data.size();
        }

		///Other functions
		void Bind() const {
		    glBindBuffer(BufferType, *ID);                                       CHECK_GL_ERRORS;
        }
		void Unbind() const {
		    glBindBuffer(BufferType, 0);                                         CHECK_GL_ERRORS;
        }
        void BindBase(const GLuint index) const {
            glBindBufferBase(BufferType, index, *ID);                            CHECK_GL_ERRORS;
        }

	protected:
	private:
	    std::shared_ptr<GLuint> ID;
        int Size;
};

/**
BT - BufferType
GLDT - GLDataType
DT - DataType
*/

template<int GLDT, typename DT>
using ArrayBuffer = Buffer<GL_ARRAY_BUFFER, GLDT, DT>;

template<int GLDT, typename DT>
using ShaderStorageBuffer = Buffer<GL_SHADER_STORAGE_BUFFER, GLDT, DT>;

typedef ArrayBuffer<GL_FLOAT, VM::vec2> Vec2ArrayBuffer;
typedef ArrayBuffer<GL_FLOAT, VM::vec4> Vec4ArrayBuffer;

typedef ShaderStorageBuffer<GL_FLOAT, float> FloatStorageBuffer;
typedef ShaderStorageBuffer<GL_FLOAT, VM::vec2> Vec2StorageBuffer;
typedef ShaderStorageBuffer<GL_FLOAT, VM::vec4> Vec4StorageBuffer;
typedef ShaderStorageBuffer<GL_UNSIGNED_INT, VM::uvec4> Uvec4StorageBuffer;
typedef ShaderStorageBuffer<GL_UNSIGNED_INT, uint> UintStorageBuffer;

typedef Buffer<GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT, uint> IndexBuffer;

template<int BT, int GLDT, typename DT>
void Buffer<BT, GLDT, DT>::SetData(const DT* data, const uint size) {
    this->Bind();
    glBufferData(BT, size, data, GL_STATIC_DRAW);                                CHECK_GL_ERRORS;
    this->Unbind();
    this->Size = size / sizeof(DT);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);                                        CHECK_GL_ERRORS;
}

template<int BT, int GLDT, typename DT>
inline void SetAttribPointer(const Buffer<BT, GLDT, DT>& buf, const int index);

template<int GLDT, typename DT>
inline void SetAttribPointer(const ArrayBuffer<GLDT, DT>& buf, const int index) {
    glVertexAttribPointer(index, DT::Components, GLDT, GL_FALSE, 0, 0); CHECK_GL_ERRORS;
}

}

#endif // BUFFER_H
