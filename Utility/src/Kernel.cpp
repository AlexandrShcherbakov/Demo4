#include "CL\Kernel.h"

namespace CL {

KernelImpl::KernelImpl(
    const cl_program program,
    const cl_command_queue queue,
    const std::string &name):
    Queue(queue)
{
    ID = clCreateKernel(program, name.c_str(), &cl_err);                         CHECK_CL(cl_err);
}

void KernelImpl::SetArgument(Buffer& buf, uint number) {
    cl_mem bufID = buf->GetID();
    CHECK_CL(clSetKernelArg(ID, number, sizeof(bufID), &bufID));
    if (Arguments.size() <= number) {
        Arguments.resize(number + 1);
    }
    Arguments[number] = buf;
}

void KernelImpl::Run(const uint size) {
    for (auto &buf: Arguments) {
        buf->AcquireGLObject();
    }

    CHECK_CL(clEnqueueNDRangeKernel(Queue, ID, 1, 0, &size, NULL, 0, NULL, NULL));
    CHECK_CL(clFinish(Queue));

    for (auto &buf: Arguments) {
        buf->ReleaseGLObject();
    }
}

KernelImpl::~KernelImpl() {
    CHECK_CL(clReleaseKernel(ID));
}

}
