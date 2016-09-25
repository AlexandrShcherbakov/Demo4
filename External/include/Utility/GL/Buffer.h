#ifndef BUFFER_H
#define BUFFER_H

#ifndef UTILITY_H_INCLUDED
#include "GL.h"
#include "VM\vec2.h"
#include "VM\vec4.h"
#endif // UTILITY_H_INCLUDED

#include <vector>
#include <map>

namespace GL {

class Buffer
{
	public:
		//Constructors and destructor
		Buffer();
		Buffer(const Buffer& buf);
		Buffer(const GLenum elType, const GLenum bufType);
		~Buffer();

		//Getters
		GLenum getElementsType() const;
        GLenum getBufferType() const;
        uint getSize() const;
        uint getComponents() const;
        inline uint getID() const {return ID;}

		//Setters
		void setData(const void* data, const uint size);
        void setData(const std::vector<uint>& data);
        void setData(const std::vector<VM::vec4>& data);
        void setData(const std::vector<VM::vec2>& data);

		//Tools
		inline bool isFloat() const { return getElementsType() == GL_FLOAT; }
		inline bool isInt() const { return getElementsType() == GL_INT; }
		inline bool isDouble() const { return getElementsType() == GL_DOUBLE; }

		inline void bind() const { glBindBuffer(getBufferType(), ID); CHECK_GL_ERRORS }
		inline void unbind() const { glBindBuffer(getBufferType(), 0); CHECK_GL_ERRORS }

	protected:
	private:
		GLuint ID;
};

}

#endif // BUFFER_H
