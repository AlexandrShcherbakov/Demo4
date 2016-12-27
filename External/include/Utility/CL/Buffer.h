#ifndef CL_BUFFER_H
#define CL_BUFFER_H

#include <memory>

#ifndef UTILITY_H_INCLUDED
#include "CL.h"
#include "Overall.h"
#include "GL.h"
#include "GL\Texture.h"
#endif // UTILITY_H_INCLUDED

namespace CL {

class BufferImpl {
    public:
        BufferImpl(
            const cl_context context,
            const cl_command_queue queue,
            const cl_mem_flags flags,
            const uint size);
        BufferImpl(
            const cl_context context,
            const cl_command_queue queue,
            const GLuint glBuf,
            const cl_mem_flags flags);
        BufferImpl(
            const cl_context context,
            const cl_command_queue queue,
            const cl_mem_flags flags,
            const GLint miplevel,
            const GLuint texID);

        ///Getters
        std::shared_ptr<float> getData() const;
        cl_mem GetID() const {
            return ID;
        }

        ///Setters
        void SetData(const void *data);
        void SetData(const void *data, const uint size);

        ///Other methods
        void AcquireGLObject();
        void ReleaseGLObject();

        ~BufferImpl();
    protected:
    private:
        cl_mem ID;
        cl_command_queue Queue;
        uint Size;
        bool glObj;
};

typedef std::shared_ptr<BufferImpl> Buffer;

}
#endif // CL_BUFFER_H
