#include "CL\Program.h"

namespace CL {

using namespace std;

void Program::loadFromFile(const std::string &filename) {
    cl_device_id device_id;
    cl_platform_id platforms[30];
    cl_uint platfcnt;

    CHECK_CL(clGetPlatformIDs(30, platforms, &platfcnt));

    CHECK_CL(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 1, &device_id, NULL));

    cl_context_properties properties[] = {
    CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(), // WGL Context
    CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(), // WGL HDC
    CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0],// OpenCL platform
    0 };

    // Create a context using the supported devices
    this->context = clCreateContext(properties, 1, &device_id, NULL, 0, &cl_err);      CHECK_CL(cl_err);

    this->queue = clCreateCommandQueue(context, device_id, 0, &cl_err);               CHECK_CL(cl_err);

    //Load source
    string source = LoadSource(filename);
    const char* c_source = source.c_str();
    uint source_len = source.size();

    //Create program
    this->program = clCreateProgramWithSource(context, 1, (const char **)&c_source, &source_len, &cl_err);     CHECK_CL(cl_err);
    delete[] c_source;
    //Compile program
    CHECK_CL(clBuildProgram(program, 0, NULL, NULL, NULL, NULL));
    size_t log_size;
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    char *log = (char *) malloc(log_size);
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
    fprintf(stderr, "%s\n", log);
    free(log);
    if (cl_err == CL_BUILD_PROGRAM_FAILURE) {
        exit(1);
    }
}

Buffer Program::createBuffer(const cl_mem_flags flags, const uint size) {
    return Buffer(context, queue, flags, size);
}
Buffer Program::createBufferFromGL(const cl_mem_flags flags, const GLuint glBuf) {
    return Buffer(context, queue, glBuf, flags);
}
Buffer Program::createBufferFromTexture(const cl_mem_flags flags, const GLint miplevel, const GLuint texID) {
    return Buffer(context, queue, flags, miplevel, texID);
}

Kernel Program::createKernel(const std::string& name) {
    return Kernel(program, queue, name);
}

}
