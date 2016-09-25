#include "GL\ShaderProgram.h"

namespace GL {

using namespace VM;

void CompileShader(const std::string &name, GLuint &shader, GLenum shaderType) {
 	//Initialization of shader
	shader = glCreateShader(shaderType);                                         CHECK_GL_ERRORS

	std::string source = LoadSource(name);

    //Compile vertex shader
    char *src = new char[source.size()];
    sprintf(src, "%s", source.c_str());
    int sourceLength = source.size();

    glShaderSource(shader, 1, (const GLchar**)&src,
                (const GLint*)&sourceLength);                                   CHECK_GL_ERRORS
    glCompileShader(shader);                                                    CHECK_GL_ERRORS

    //Check whether the compilation is successful
    if (ShaderStatus(shader, GL_COMPILE_STATUS) != GL_TRUE) {
        std::cerr << "BUGURT!!! Line: " << __LINE__ << " File: " << name << std::endl;
        exit(1);
    }
}

ShaderProgram::ShaderProgram() {
    ID = glCreateProgram();                                                      CHECK_GL_ERRORS
}

ShaderProgram::ShaderProgram(const std::string& filename) {
    ID = glCreateProgram();                                                      CHECK_GL_ERRORS
    loadFromFile(filename);
}

void ShaderProgram::loadFromFile(const std::string& filename) {
    GLuint vertSh, fragSh;
    CompileShader("shaders/" + filename + ".vert", vertSh, GL_VERTEX_SHADER);
    CompileShader("shaders/" + filename + ".frag", fragSh, GL_FRAGMENT_SHADER);

    glAttachShader(ID, vertSh);                                                  CHECK_GL_ERRORS
    glAttachShader(ID, fragSh);                                                  CHECK_GL_ERRORS

    glLinkProgram(ID);                                                           CHECK_GL_ERRORS

    if (ShaderProgramStatus(ID, GL_LINK_STATUS) != GL_TRUE) {
        std::cerr << "BUGURT!!! Line: " <<  __LINE__ << std::endl;
        exit(1);
    }
}

void ShaderProgram::setUniform(const std::string& name, int value) {
    glUseProgram(ID);                                                            CHECK_GL_ERRORS
	GLint location = glGetUniformLocation(ID, name.c_str());                     CHECK_GL_ERRORS
    glUniform1i(location, value);                                                CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

void ShaderProgram::setUniform(const std::string& name, float value) {
    glUseProgram(ID);                                                            CHECK_GL_ERRORS
	GLint location = glGetUniformLocation(ID, name.c_str());                     CHECK_GL_ERRORS
    glUniform1f(location, value);                                                CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

void ShaderProgram::setUniform(const std::string& name, mat4 value) {
    glUseProgram(ID);                                                            CHECK_GL_ERRORS
    GLint location = glGetUniformLocation(ID, name.c_str());                     CHECK_GL_ERRORS
    glUniformMatrix4fv(location, 1, GL_TRUE, value.data().data());               CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

void ShaderProgram::setUniform(const std::string& name, vec4 value) {
    glUseProgram(ID);                                                            CHECK_GL_ERRORS
    GLint location = glGetUniformLocation(ID, name.c_str());                     CHECK_GL_ERRORS
    glUniform4f(location, value.x, value.y, value.z, value.w);                   CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

void ShaderProgram::setUniform(const std::string& name, vec3 value) {
    glUseProgram(ID);                                                            CHECK_GL_ERRORS
    GLint location = glGetUniformLocation(ID, name.c_str());                     CHECK_GL_ERRORS
    glUniform3f(location, value.x, value.y, value.z);                            CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

void ShaderProgram::setLight(const std::string& name, SpotLightSource& light) {
    setUniform(name, light.getMatrix());
    setUniform("lightDir", light.direction);
    setUniform("lightColor", light.color);
    setUniform("lightPos", light.position);
    setUniform("innerCos", (float)cos(light.innerCone));
    setUniform("outterCos", (float)cos(light.angle));
}

void ShaderProgram::setCamera(const std::string& name, ViewPoint& camera) {
    setUniform(name, camera.getMatrix());
}

}
