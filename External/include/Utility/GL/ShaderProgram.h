#ifndef SHADER_H
#define SHADER_H

#include <memory>

#ifndef UTILITY_H_INCLUDED
#include "GL\SpotLightSource.h"
#include "GL\DirectionalLightSource.h"
#include "GL\Camera.h"
#include "GL.h"
#endif // UTILITY_H_INCLUDED

namespace GL {

class ShaderProgram
{
	public:
		///Constructor
		ShaderProgram() {
            ID = std::shared_ptr<GLuint>(new GLuint, [](GLuint* ID) {glDeleteProgram(*ID);});
            *ID = glCreateProgram();
		}

		///Getters
        GLuint GetAttribLocation(const std::string& name) const {
			return glGetAttribLocation(*ID, name.c_str());                        CHECK_GL_ERRORS;
        }

        ///Setters
        template<typename T>
		void SetUniform(const std::string& name, T value) const;

		///Other functions
		void LoadFromFile(const std::string& filename) const;
		void Bind() const {
            glUseProgram(*ID);                                                   CHECK_GL_ERRORS;
		}
		void Unbind() const {
            glUseProgram(0);                                                     CHECK_GL_ERRORS;
		}

	protected:
	private:
        std::shared_ptr<GLuint> ID;
};

template<typename T>
void SetUniformValueByLocation(const GLuint location, const T& value);

void SetUniformValueByLocation(const GLuint location, const int& value);
void SetUniformValueByLocation(const GLuint location, const float& value);
void SetUniformValueByLocation(const GLuint location, const VM::mat4& value);
void SetUniformValueByLocation(const GLuint location, const VM::vec4& value);
void SetUniformValueByLocation(const GLuint location, const VM::vec3& value);

template<typename T>
void ShaderProgram::SetUniform(const std::string& name, const T value) const {
    this->Bind();
    GLuint location = glGetUniformLocation(*ID, name.c_str());                    CHECK_GL_ERRORS;
    SetUniformValueByLocation(location, value);
    this->Unbind();
}

void SetLightToProgram(
    const ShaderProgram& program,
    const std::string& name,
    const SpotLightSource& light
);

void SetLightToProgram(
    const ShaderProgram& program,
    const std::string& name,
    const DirectionalLightSource& light
);

void SetCameraToProgram(
    const ShaderProgram& program,
    const std::string& name,
    const ViewPoint& camera
);

}
#endif // SHADER_H
