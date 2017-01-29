#include "ComputeShader.h"

namespace GL {
void ComputeShader::LoadFromFile(const std::string& filename) const {
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);                        CHECK_GL_ERRORS;

    std::string shaderSource = LoadSource("../shaders/" + filename + ".vert");
    const char* shaderChars = shaderSource.c_str();
    glShaderSource(shader, 1, &shaderChars, NULL);                            CHECK_GL_ERRORS;
    glCompileShader(shader);                                                  CHECK_GL_ERRORS;

    ShaderStatus(shader, GL_COMPILE_STATUS);

    glAttachShader(*ID, shader);                                              CHECK_GL_ERRORS;
    glLinkProgram(*ID);                                                       CHECK_GL_ERRORS;

    ShaderProgramStatus(*ID, GL_LINK_STATUS);
}

void ComputeShader::Run(const uint x, const uint y, const uint z) const {
    Bind();
    glDispatchCompute(x, y, z);
    Unbind();
}
}
