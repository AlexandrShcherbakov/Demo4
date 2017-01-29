#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include <memory>

#ifndef UTILITY_H_INCLUDED
#include "GL.h"
#include "GL/ShaderProgram.h"
#endif // UTILITY_H_INCLUDED

namespace GL {

class ComputeShader
{
    public:
        ComputeShader() {
            ID = std::shared_ptr<GLuint>(new GLuint, [](GLuint* ID) {glDeleteProgram(*ID);});
            *ID = glCreateProgram();
        }

        ///Getters
        GLuint GetUniformLocation(const std::string& name) const {
			return glGetUniformLocation(*ID, name.c_str());                        CHECK_GL_ERRORS;
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
		void Run(const uint x, const uint y, const uint z) const;

    private:
        std::shared_ptr<GLuint> ID;

};

template<typename T>
void ComputeShader::SetUniform(const std::string& name, const T value) const {
    this->Bind();
    GLuint location = glGetUniformLocation(*ID, name.c_str());                    CHECK_GL_ERRORS;
    SetUniformValueByLocation(location, value);
    this->Unbind();
}

}

#endif // COMPUTESHADER_H
