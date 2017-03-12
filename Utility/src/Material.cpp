#include "Material.h"

namespace GL {

using namespace VM;

Material::Material(const vec4& color, Texture<GL_TEXTURE_2D, GL_RGBA>* tex) {
	this->color = color;
    this->tex = tex;
}

void Material::bindMaterial(
	ShaderProgram& program,
	const std::string& color_name,
	const std::string& tex_name
) {
	if (tex) tex->BindToShader(program, tex_name);
	program.SetUniform(color_name, color);
}

}
