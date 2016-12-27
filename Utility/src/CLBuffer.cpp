#include "CL\Buffer.h"

namespace CL {

BufferImpl::BufferImpl(
    const cl_context context,
    const cl_command_queue queue,
    const cl_mem_flags flags,
    const uint size):
    Queue(queue),
    Size(size),
    glObj(false)
{
    ID = clCreateBuffer(context, flags, Size, NULL, &cl_err);                    CHECK_CL(cl_err);
}

BufferImpl::BufferImpl(
    const cl_context context,
    const cl_command_queue queue,
    const GLuint glBuf,
    const cl_mem_flags flags):
    Queue(queue),
    glObj(true)
{
    ID = clCreateFromGLBuffer(context, flags, glBuf, &cl_err);                   CHECK_CL(cl_err);
}

BufferImpl::BufferImpl(
    const cl_context context,
    const cl_command_queue queue,
    const cl_mem_flags flags,
    const GLint miplevel,
    const GLuint texID):
    Queue(queue),
    glObj(true)
{
    ID = clCreateFromGLTexture2D(context, flags, GL_TEXTURE_2D, miplevel, texID, &cl_err); CHECK_CL(cl_err);
}

void BufferImpl::SetData(const void *data) {
    CHECK_CL(clEnqueueWriteBuffer(Queue, ID, CL_TRUE, 0, Size, data, 0, NULL, NULL));
}

void BufferImpl::SetData(const void *data, const uint size) {
    CHECK_CL(clEnqueueWriteBuffer(Queue, ID, CL_TRUE, 0, size, data, 0, NULL, NULL));
}

void BufferImpl::AcquireGLObject() {
    if (glObj) {
        CHECK_CL(clEnqueueAcquireGLObjects(Queue, 1, &ID, 0, 0, 0));
    }
}

void BufferImpl::ReleaseGLObject() {
    if (glObj) {
        CHECK_CL(clEnqueueReleaseGLObjects(Queue, 1, &ID, 0, 0, 0));
    }
}

BufferImpl::~BufferImpl() {
    CHECK_CL(clReleaseMemObject(ID));
}

}
