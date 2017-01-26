#include "Mesh.h"

namespace GL {

Mesh::Mesh() {
    glGenVertexArrays(1, &ID);                                                   CHECK_GL_ERRORS
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &ID);                                                CHECK_GL_ERRORS
}

void Mesh::bindIndicesBuffer(const IndexBuffer& buf) {
    glBindVertexArray(ID);                                                       CHECK_GL_ERRORS
	buf.Bind();
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    this->size = buf.GetSize();
}

void Mesh::Draw(const uint count, Framebuffer* target) {
    for (auto &tex: textures)
        tex.second->bindTexture(*program, tex.first);
    for (auto &light: spots) {
        SetLightToProgram(*program, light.first, *(light.second));
    }
    for (auto &light: directionals) {
        SetLightToProgram(*program, light.first, *(light.second));
    }
    SetCameraToProgram(*program, "camera", *camera);
    program->Bind();
    glBindVertexArray(ID);                                                       CHECK_GL_ERRORS
    if (target != nullptr) target->Bind();
    glDrawArrays(GL_TRIANGLES, 0, count);                                        CHECK_GL_ERRORS
    if (target != nullptr) target->Unbind();
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    program->Unbind();
}

void Mesh::DrawWithIndices(const GLenum mode, Framebuffer *target) {
    for (auto &tex: textures) {
        tex.second->bindTexture(*program, tex.first);
    }
    for (auto &light: spots) {
        SetLightToProgram(*program, light.first, *(light.second));
    }
    for (auto &light: directionals) {
        SetLightToProgram(*program, light.first, *(light.second));
    }
    SetCameraToProgram(*program, "camera", *camera);
    material.bindMaterial(*program);
    program->Bind();
    glBindVertexArray(ID);                                                       CHECK_GL_ERRORS
    if (target != nullptr) target->Bind();
    glDrawElements(mode, size, GL_UNSIGNED_INT, 0);                              CHECK_GL_ERRORS
    if (target != nullptr) target->Unbind();
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    program->Unbind();
}


}
