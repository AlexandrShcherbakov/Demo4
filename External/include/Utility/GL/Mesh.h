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
		void bindBuffer(const Buffer& buf, const uint index);
		void bindBuffer(const Buffer& buf, ShaderProgram prog, const std::string& name);
		void bindIndicesBuffer(const Buffer& buf);

		inline void setShaderProgram(ShaderProgram& program) {this->program = &program;}
        inline void setCamera(ViewPoint* camera) {this->camera = camera;}
        inline void setMaterial(Material& material) {this->material = material;}
        inline void addTexture(const std::string &name, Texture & texture) {textures[name] = &texture;}
        inline void addLight(const std::string &name, SpotLightSource & light) {spots[name] = &light;}
        inline void addLight(const std::string &name, DirectionalLightSource & light) {directionals[name] = &light;}
        inline bool texturedMaterial() const {return material.hasTexture();}
        inline Material getMaterial() const {return material;}

        void Draw(const uint count, RWTexture* target=nullptr);
        void DrawWithIndices(RWTexture* target=nullptr);

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

}

#endif // MESH_H
