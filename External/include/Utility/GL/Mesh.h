#ifndef MESH_H
#define MESH_H

#ifndef UTILITY_H_INCLUDED
#include "GL.h"
#include "GL\Buffer.h"
#include "GL\ShaderProgram.h"
#include "GL\Texture.h"
#include "GL\Framebuffer.h"
#include "GL\Material.h"
#endif // UTILITY_H_INCLUDED

namespace GL {

class Mesh
{
	public:
	    ///Constructors
		Mesh();

        ///Getters

        ///Setters
        void SetShaderProgram(ShaderProgram& program) {
            Program = &program;
        }
        void SetCamera(ViewPoint& camera) {
            Camera = &camera;
        }
        void SetAmbientColor(const VM::vec4& color) {
            AmbientColor = color;
        }

        ///Other functions
		template<int GLDT, typename DT>
		void BindBuffer(const ArrayBuffer<GLDT, DT>& buf, const std::string& name);
		void BindIndicesBuffer(const IndexBuffer& buf);

        void AddTexture(const std::string &name, Texture* texture) {
            Textures[name] = texture;
        }
        void AddLight(const std::string &name, SpotLightSource & light) {
            Spots[name] = &light;
        }
        void AddLight(const std::string &name, DirectionalLightSource & light) {
            Directionals[name] = &light;
        }

        void DrawWithIndices(const GLenum mode=GL_TRIANGLES, Framebuffer* target=nullptr);

        bool HasTextures() const {
            return !Textures.empty();
        }

	protected:
	private:
		std::shared_ptr<GLuint> ID;
		std::map<std::string, Texture*> Textures;
		std::map<std::string, SpotLightSource*> Spots;
		std::map<std::string, DirectionalLightSource*> Directionals;
        ShaderProgram* Program;
        GL::ViewPoint* Camera;
        uint Size;
        VM::vec4 AmbientColor;
};

template<int GLDT, typename DT>
void Mesh::BindBuffer(const ArrayBuffer<GLDT, DT>& buf, const std::string& name) {
	glBindVertexArray(*ID);                                                      CHECK_GL_ERRORS;
	buf.Bind();
	GLuint index = Program->GetAttribLocation(name);
    glEnableVertexAttribArray(index);                                            CHECK_GL_ERRORS;
    SetAttribPointer(buf, index);
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS;
}

}

#endif // MESH_H
