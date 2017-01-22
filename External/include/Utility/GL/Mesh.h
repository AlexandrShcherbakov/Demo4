#ifndef MESH_H
#define MESH_H

#ifndef UTILITY_H_INCLUDED
#include "GL.h"
#include "GL\Buffer.h"
#include "GL\ShaderProgram.h"
#include "GL\Texture.h"
#include "GL\RWTexture.h"
#include "GL\Material.h"
#endif // UTILITY_H_INCLUDED

namespace GL {

class Mesh
{
	public:
		Mesh();
		~Mesh();
        template<int GLDT, typename DT>
		void bindBuffer(const ArrayBuffer<GLDT, DT>& buf, const uint index);
		template<int GLDT, typename DT>
		void bindBuffer(const ArrayBuffer<GLDT, DT>& buf, ShaderProgram prog, const std::string& name);
		void bindIndicesBuffer(const IndexBuffer& buf);

		inline void setShaderProgram(ShaderProgram& program) {this->program = &program;}
        inline void setCamera(ViewPoint* camera) {this->camera = camera;}
        inline void setMaterial(Material& material) {this->material = material;}
        inline void addTexture(const std::string &name, Texture & texture) {textures[name] = &texture;}
        inline void addLight(const std::string &name, SpotLightSource & light) {spots[name] = &light;}
        inline void addLight(const std::string &name, DirectionalLightSource & light) {directionals[name] = &light;}
        inline bool texturedMaterial() const {return material.hasTexture();}
        inline Material getMaterial() const {return material;}

        void Draw(const uint count, RWTexture* target=nullptr);
        void DrawWithIndices(const GLenum mode=GL_TRIANGLES, RWTexture* target=nullptr);

	protected:
	private:
		GLuint ID;
		std::map<std::string, Texture *> textures;
		std::map<std::string, SpotLightSource *> spots;
		std::map<std::string, DirectionalLightSource *> directionals;
        ViewPoint * camera;
        ShaderProgram * program;
        uint size;
        Material material;
};

template<int GLDT, typename DT>
void Mesh::bindBuffer(const ArrayBuffer<GLDT, DT>& buf, const uint index) {
    glBindVertexArray(ID);                                                       CHECK_GL_ERRORS
	buf.Bind();
	uint components = buf.getComponents();
    if (buf.isFloat())
		glVertexAttribPointer(index, components, buf.getElementsType(), GL_FALSE, 0, 0);  CHECK_GL_ERRORS
	if (buf.isInt())
		glVertexAttribIPointer(index, components, buf.getElementsType(), 0, 0);  CHECK_GL_ERRORS
	if (buf.isDouble())
		glVertexAttribLPointer(index, components, buf.getElementsType(), 0, 0);  CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
}

template<int GLDT, typename DT>
void Mesh::bindBuffer(const ArrayBuffer<GLDT, DT>& buf, ShaderProgram prog, const std::string& name) {
	glBindVertexArray(ID);                                                       CHECK_GL_ERRORS
	buf.Bind();
	GLuint index = glGetAttribLocation(prog.ID, name.c_str());                   CHECK_GL_ERRORS
    glEnableVertexAttribArray(index);                                            CHECK_GL_ERRORS
    uint components = buf.getComponents();
    SetAttribPointer(buf, index);
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
}

void Mesh::bindIndicesBuffer(const IndexBuffer& buf) {
    glBindVertexArray(ID);                                                       CHECK_GL_ERRORS
	buf.Bind();
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    this->size = buf.getSize();
}


}

#endif // MESH_H
