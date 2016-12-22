#ifndef SHADER_H
#define SHADER_H

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
		ShaderProgram();
		ShaderProgram(const std::string& filename);
		void setUniform(const std::string& name, VM::mat4 value);
		void setUniform(const std::string& name, VM::vec4 value);
		void setUniform(const std::string& name, VM::vec3 value);
		void setUniform(const std::string& name, int value);
		void setUniform(const std::string& name, float value);
		void loadFromFile(const std::string& filename);
        void setLight(const std::string& name, SpotLightSource& light);
        void setLight(const std::string& name, DirectionalLightSource& light);
        void setCamera(const std::string& name, ViewPoint& camera);

		GLuint ID;

	protected:
	private:
};

}
#endif // SHADER_H
