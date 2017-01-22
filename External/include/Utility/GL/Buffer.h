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
		GLenum getElementsType() const;
        GLenum getBufferType() const;
        uint getSize() const;
        uint getComponents() const;
        inline uint getID() const {return ID;}

		///Setters
		void SetData(const DataType* data, const uint size) const;
        void SetData(const std::vector<DataType>& data) const {
            SetData(data.data(), data.size() * sizeof(DataType));
        }

		///Other functions
		void Bind() const {
		    glBindBuffer(BufferType, *ID);                                       CHECK_GL_ERRORS;
        }
		void Unbind() const {
		    glBindBuffer(BufferType, 0);                                         CHECK_GL_ERRORS;
        }

	protected:
	private:
	    std::shared_ptr<GLuint> ID;
};

/**
BT - BufferType
GLDT - GLDataType
DT - DataType
*/

template<int GLDT, typename DT>
using ArrayBuffer = Buffer<GL_ARRAY_BUFFER, GLDT, DT>;

typedef ArrayBuffer<GL_FLOAT, VM::vec2> Vec2ArrayBuffer;
typedef ArrayBuffer<GL_FLOAT, VM::vec4> Vec4ArrayBuffer;
typedef Buffer<GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT, uint> IndexBuffer;

template<int BT, int GLDT, typename DT>
void Buffer<BT, GLDT, DT>::SetData(const DT* data, const uint size) const {
    this->Bind();
    glBufferData(BT, size, data, GL_STATIC_DRAW);                                CHECK_GL_ERRORS;
    this->Unbind();
}

template<int BT, int GLDT, typename DT>
inline void SetAttribPointer(Buffer<BT, GLDT, DT>& buf, const int index);

template<int GLDT, typename DT>
inline void SetAttribPointer(ArrayBuffer<GLDT, DT>& buf, const int index) {
    glVertexAttribPointer(index, DT::Components, GLDT, GL_FALSE, 0, 0); CHECK_GL_ERRORS;
}

}

#endif // BUFFER_H
