#include "CL\Buffer.h"

namespace CL {

BufferImpl::BufferImpl() {
}

BufferImpl::BufferImpl(
    const cl_context context,
    const cl_command_queue queue,
    const cl_mem_flags flags,
    const uint size) {

    this->context = context;
    this->queue = queue;
    this->size = size;
    this->gl_obj = false;
    this->id = clCreateBuffer(context, flags, size, NULL, &cl_err);              CHECK_CL(cl_err);
}

BufferImpl::BufferImpl(
    const cl_context context,
    const cl_command_queue queue,
    const GLuint glBuf,
    const cl_mem_flags flags) {

    this->context = context;
    this->queue = queue;
    this->gl_obj = true;
    this->id = clCreateFromGLBuffer(context, flags, glBuf, &cl_err);             CHECK_CL(cl_err);
}

BufferImpl::BufferImpl(
    const cl_context context,
    const cl_command_queue queue,
    const cl_mem_flags flags,
    const GLint miplevel,
    const GLuint texID) {

    this->context = context;
    this->queue = queue;
    this->gl_obj = true;
    this->id = clCreateFromGLTexture2D(
        context, flags, GL_TEXTURE_2D,
        miplevel, texID, &cl_err);                                               CHECK_CL(cl_err);
}

void BufferImpl::loadData(const void *data) {
    CHECK_CL(clEnqueueWriteBuffer(queue, id, CL_TRUE, 0, size, data, 0, NULL, NULL));
}

void BufferImpl::loadData(const void *data, const uint size) {
    CHECK_CL(clEnqueueWriteBuffer(queue, id, CL_TRUE, 0, size, data, 0, NULL, NULL));
}

std::shared_ptr<float> BufferImpl::getData() const {
    std::shared_ptr<float> data(new float[size / sizeof(float)], [](float* p){ delete[] p; });
    CHECK_CL(clEnqueueReadBuffer(queue, id, CL_TRUE, 0, size, data.get(), 0, NULL, NULL));
    return data;
}

void BufferImpl::acquireGLObject() {
    if (gl_obj)
        CHECK_CL(clEnqueueAcquireGLObjects(queue, 1, &this->id, 0, 0, 0));
}

void BufferImpl::releaseGLObject() {
    if (gl_obj)
        CHECK_CL(clEnqueueReleaseGLObjects(queue, 1, &this->id, 0, 0, 0));
}

}
