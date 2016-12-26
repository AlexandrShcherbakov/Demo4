#ifndef KERNEL_H
#define KERNEL_H

#ifndef UTILITY_H_INCLUDED
#include "CL.h"
#include "CL\Buffer.h"
#include "Overall.h"
#include "GL.h"
#endif // UTILITY_H_INCLUDED

namespace CL {

class Kernel
{
    public:
        Kernel();
        Kernel(const cl_program program, const cl_command_queue queue, const std::string &name);
        void addArgument(Buffer& buf, const uint number);
        void run(const uint size);
    protected:
    private:
        cl_kernel id;
        std::map<uint, Buffer> buffers;
        cl_program program;
        cl_command_queue queue;
};

}

#endif // KERNEL_H
