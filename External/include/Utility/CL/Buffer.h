#ifndef CL_BUFFER_H
#define CL_BUFFER_H

#ifndef UTILITY_H_INCLUDED
#include "CL.h"
#include "Overall.h"
#include "GL.h"
#include "GL\Texture.h"
#endif // UTILITY_H_INCLUDED

namespace CL {

class Buffer {
    public:
        Buffer();
        Buffer(
            const cl_context context,
            const cl_command_queue queue,
            const cl_mem_flags flags,
            const uint size);
        Buffer(
            const cl_context context,
            const cl_command_queue queue,
            const GLuint glBuf,
            const cl_mem_flags flags);
        Buffer(
            const cl_context context,
            const cl_command_queue queue,
            const cl_mem_flags flags,
            const GLint miplevel,
            const GLuint texID);
        void loadData(const void *data);
        void loadData(const void *data, const uint size);
        void getData(void *data, const uint size);

        void acquireGLObject();
        void releaseGLObject();
        cl_mem id;
    protected:
    private:
        uint size;
        bool gl_obj;
        cl_context context;
        cl_command_queue queue;
};

}
#endif // CL_BUFFER_H
