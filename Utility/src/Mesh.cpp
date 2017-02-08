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

void Mesh::Draw(const GLuint size, const GLenum mode, Framebuffer* target) {
    for (auto tex: Textures) {
        tex.second->BindToShader(*Program, tex.first);
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
    for (auto tex: Textures) {
        tex.second->BindToShader(*Program, tex.first);
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
