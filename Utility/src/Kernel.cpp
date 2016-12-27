#include "CL\Kernel.h"

namespace CL {

KernelImpl::KernelImpl(const cl_program program, const cl_command_queue queue, const std::string &name) {
    this->program = program;
    this->queue = queue;
    this->id = clCreateKernel(program, name.c_str(), &cl_err);              CHECK_CL(cl_err);
}

void KernelImpl::addArgument(Buffer& buf, uint number) {
    cl_mem bufID = buf->GetID();
    CHECK_CL(clSetKernelArg(this->id, number, sizeof(bufID), &bufID));
    buffers[number] = buf;
}

void KernelImpl::run(const uint size) {
    for (auto &buf: buffers) {
        buf.second->AcquireGLObject();
    }

    CHECK_CL(clEnqueueNDRangeKernel(queue, this->id, 1, 0, &size, NULL, 0, NULL, NULL));
    CHECK_CL(clFinish(queue));

    for (auto &buf: buffers) {
        buf.second->ReleaseGLObject();
    }
}

KernelImpl::~KernelImpl() {
    CHECK_CL(clReleaseKernel(id));
}

}
