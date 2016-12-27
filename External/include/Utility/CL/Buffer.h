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
        cl_mem GetID() const {
            return ID;
        }
        cl_command_queue GetQueue() const {
            return Queue;
        }
        uint GetSize() const {
            return Size;
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

template<typename T>
std::shared_ptr<T> ExtractData(const Buffer& buf) {
    std::shared_ptr<T> data(new T[buf->GetSize() / sizeof(T)], std::default_delete<T>());
    CHECK_CL(clEnqueueReadBuffer(buf->GetQueue(), buf->GetID(), CL_TRUE, 0, buf->GetSize(), data.get(), 0, NULL, NULL));
    return data;
}

}
#endif // CL_BUFFER_H
