#include "CL\Kernel.h"

namespace CL {

Kernel::Kernel() {}

Kernel::Kernel(const cl_program program, const cl_command_queue queue, const std::string &name) {
    this->program = program;
    this->queue = queue;
    this->id = clCreateKernel(program, name.c_str(), &cl_err);              CHECK_CL(cl_err);
}

void Kernel::addArgument(Buffer& buf, uint number) {
    CHECK_CL(clSetKernelArg(this->id, number, sizeof(buf->id), &(buf->id)));
    buffers[number] = buf;
}

void Kernel::run(const uint size) {
    for (auto &buf: buffers) {
        buf.second->acquireGLObject();
    }

    CHECK_CL(clEnqueueNDRangeKernel(queue, this->id, 1, 0, &size, NULL, 0, NULL, NULL));
    CHECK_CL(clFinish(queue));

    for (auto &buf: buffers) {
        buf.second->releaseGLObject();
    }
}

}
