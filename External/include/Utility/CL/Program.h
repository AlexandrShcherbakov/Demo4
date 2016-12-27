#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>

#ifndef UTILITY_H_INCLUDED
#include "CL.h"
#include "CL\Buffer.h"
#include "CL\Kernel.h"
#include "Overall.h"
#include "GL\Buffer.h"
#endif // UTILITY_H_INCLUDED

namespace CL {

class Program
{
    public:
        void LoadFromFile(const std::string& filename);
        Buffer CreateBuffer(const uint size, const cl_mem_flags flags=CL_MEM_READ_WRITE);
        Buffer CreateBufferFromGL(const GLuint glBuf, const cl_mem_flags flags=CL_MEM_READ_WRITE);
        Buffer CreateBufferFromTexture(const GLint miplevel, const GLuint texID, const cl_mem_flags flags=CL_MEM_READ_WRITE);
        Kernel CreateKernel(const std::string& name);
    protected:
    private:
    cl_context Context;
    cl_program Program;
    cl_command_queue Queue;
};

}

#endif // PROGRAM_H
