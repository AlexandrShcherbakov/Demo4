#include "GL\ShaderProgram.h"

namespace GL {

void CompileShader(const std::string &name, GLuint &shader, GLenum shaderType) {
	shader = glCreateShader(shaderType);                                         CHECK_GL_ERRORS;

	std::string source = LoadSource(name);

    const char *src = source.c_str();
    int sourceLength = source.size();

    glShaderSource(
        shader, 1, static_cast<const GLchar**>(&src),
        static_cast<const GLint*>(&sourceLength)
    );                                                                           CHECK_GL_ERRORS;
    glCompileShader(shader);                                                     CHECK_GL_ERRORS;

    if (ShaderStatus(shader, GL_COMPILE_STATUS) != GL_TRUE) {
        std::cerr << "BUGURT!!! Line: " << __LINE__ << " File: " << name << std::endl;
        exit(1);
    }
}

void ShaderProgram::LoadFromFile(const std::string& filename) const {
    GLuint vertSh, fragSh;
    CompileShader("../shaders/" + filename + ".vert", vertSh, GL_VERTEX_SHADER);
    CompileShader("../shaders/" + filename + ".frag", fragSh, GL_FRAGMENT_SHADER);

    glAttachShader(*ID, vertSh);                                                 CHECK_GL_ERRORS
    glAttachShader(*ID, fragSh);                                                 CHECK_GL_ERRORS

    glLinkProgram(*ID);                                                          CHECK_GL_ERRORS

    if (ShaderProgramStatus(*ID, GL_LINK_STATUS) != GL_TRUE) {
        std::cerr << "BUGURT!!! Line: " <<  __LINE__ << std::endl;
        exit(1);
    }
}

void SetUniformValueByLocation(const GLuint location, const int& value) {
    glUniform1i(location, value);                                                CHECK_GL_ERRORS
}

void SetUniformValueByLocation(const GLuint location, const float& value) {
    glUniform1f(location, value);                                                CHECK_GL_ERRORS
}

void SetUniformValueByLocation(const GLuint location, const VM::mat4& value) {
    glUniformMatrix4fv(location, 1, GL_TRUE, value.data().data());               CHECK_GL_ERRORS
}

void SetUniformValueByLocation(const GLuint location, const VM::vec4& value) {
    glUniform4f(location, value.x, value.y, value.z, value.w);                   CHECK_GL_ERRORS
}

void SetUniformValueByLocation(const GLuint location, const VM::vec3& value) {
    glUniform3f(location, value.x, value.y, value.z);                            CHECK_GL_ERRORS
}

void SetLightToProgram(
    const ShaderProgram& program,
    const std::string& name,
    const SpotLightSource& light
) {
    program.SetUniform(name, light.getMatrix());
    program.SetUniform("lightDir", light.direction);
    program.SetUniform("lightColor", light.color);
    program.SetUniform("lightPos", light.position);
    program.SetUniform("innerCos", (float)cos(light.innerCone));
    program.SetUniform("outterCos", (float)cos(light.angle));
}

void SetLightToProgram(
    const ShaderProgram& program,
    const std::string& name,
    const DirectionalLightSource& light
) {
    program.SetUniform(name, light.getMatrix());
    program.SetUniform("lightDir", light.direction);
    program.SetUniform("lightColor", light.GetColor());
    program.SetUniform("lightPos", light.position);
    program.SetUniform("innerRadius", light.GetInnerRadius());
    program.SetUniform("outterRadius", light.GetOutterRadius());
}

void SetCameraToProgram(
    const ShaderProgram& program,
    const std::string& name,
    const ViewPoint& camera
) {
    program.SetUniform(name, camera.getMatrix());
}

}
