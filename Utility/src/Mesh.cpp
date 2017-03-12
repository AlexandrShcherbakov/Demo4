#include "Mesh.h"

namespace GL {

Mesh::Mesh() {
    ID = std::shared_ptr<GLuint>(
        new GLuint,
        [](GLuint* ID) {glDeleteVertexArrays(1, ID);}
    );
    glGenVertexArrays(1, ID.get());                                              CHECK_GL_ERRORS;
    Size = 0;
}

void Mesh::BindIndicesBuffer(const IndexBuffer& buf) {
    glBindVertexArray(*ID);                                                      CHECK_GL_ERRORS;
	buf.Bind();
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS;
    Size = buf.GetSize();
}

template<int IMG_FORMAT, int INTERNAL_FORMAT>
void BindTextureToTextureUnit(
    const Texture<IMG_FORMAT, INTERNAL_FORMAT>& texture,
    const int textureUnit
) {
    glActiveTexture(GL_TEXTURE0 + textureUnit);                   CHECK_GL_ERRORS;
    texture.Bind();
}

void BindTextureUnitToShader(
    const ShaderProgram& program,
    const std::string& name,
    const int textureUnit
) {
    program.Bind();
    program.SetUniform(name, textureUnit);
}

void Mesh::Draw(const GLuint size, const GLenum mode, Framebuffer* target) {
    uint texUnit = 0;
    for (auto tex: Textures) {
        BindTextureToTextureUnit(*tex.second, texUnit);
        BindTextureUnitToShader(*Program, tex.first, texUnit++);
    }
    for (auto tex: SMaps) {
        BindTextureToTextureUnit(*tex.second, texUnit);
        BindTextureUnitToShader(*Program, tex.first, texUnit++);
    }
    for (auto light: Spots) {
        SetLightToProgram(*Program, light.first, *(light.second));
    }
    for (auto light: Directionals) {
        SetLightToProgram(*Program, light.first, *(light.second));
    }
    SetCameraToProgram(*Program, "camera", *Camera);
    Program->SetUniform("material_color", AmbientColor);
    Program->Bind();
    glBindVertexArray(*ID);                                                      CHECK_GL_ERRORS;
    if (target != nullptr) target->Bind();
    glDrawArrays(mode, 0, size);                                                 CHECK_GL_ERRORS;
    glFinish();                                                                  CHECK_GL_ERRORS;
    if (target != nullptr) target->Unbind();
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS;
    Program->Unbind();
}

void Mesh::DrawWithIndices(const GLenum mode, Framebuffer *target) {
    uint texUnit = 0;
    for (auto tex: Textures) {
        BindTextureToTextureUnit(*tex.second, texUnit);
        BindTextureUnitToShader(*Program, tex.first, texUnit++);
    }
    for (auto tex: SMaps) {
        BindTextureToTextureUnit(*tex.second, texUnit);
        BindTextureUnitToShader(*Program, tex.first, texUnit++);
    }
    for (auto light: Spots) {
        SetLightToProgram(*Program, light.first, *(light.second));
    }
    for (auto light: Directionals) {
        SetLightToProgram(*Program, light.first, *(light.second));
    }
    SetCameraToProgram(*Program, "camera", *Camera);
    Program->SetUniform("material_color", AmbientColor);
    Program->Bind();
    glBindVertexArray(*ID);                                                      CHECK_GL_ERRORS;
    if (target != nullptr) target->Bind();
    glDrawElements(mode, Size, GL_UNSIGNED_INT, 0);                              CHECK_GL_ERRORS;
    glFinish();                                                                  CHECK_GL_ERRORS;
    if (target != nullptr) target->Unbind();
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS;
    Program->Unbind();
}

}
