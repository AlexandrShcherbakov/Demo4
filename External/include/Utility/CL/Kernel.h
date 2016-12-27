#ifndef KERNEL_H
#define KERNEL_H

#ifndef UTILITY_H_INCLUDED
#include "CL.h"
#include "CL\Buffer.h"
#include "Overall.h"
#include "GL.h"
#endif // UTILITY_H_INCLUDED

namespace CL {

class KernelImpl
{
    public:
        KernelImpl(const cl_program program, const cl_command_queue queue, const std::string &name);

        ///Setters
        void SetArgument(Buffer& buf, const uint number);

        ///Other methods
        void Run(const uint size);
        void UnsetArguments() {
            Arguments.clear();
        }

        ~KernelImpl();
    protected:
    private:
        cl_command_queue Queue;
        cl_kernel ID;
        std::vector<Buffer> Arguments;
};

typedef std::shared_ptr<KernelImpl> Kernel;

}

#endif // KERNEL_H
