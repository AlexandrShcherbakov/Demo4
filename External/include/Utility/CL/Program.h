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
        void loadFromFile(const std::string& filename);
        Buffer createBuffer(const cl_mem_flags flags, const uint size);
        Buffer createBufferFromGL(const cl_mem_flags flags, const GLuint glBuf);
        Buffer createBufferFromTexture(const cl_mem_flags flags, const GLint miplevel, const GLuint texID);
        Kernel createKernel(const std::string& name);
    protected:
    private:
    cl_context context;
    cl_program program;
    cl_command_queue queue;
};

}

#endif // PROGRAM_H
