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

void Mesh::Draw(const uint count, RWTexture *target) {
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
    if (target != nullptr) target->bindForDraw();
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, count);                                        CHECK_GL_ERRORS
    if (target != nullptr) target->unbind();
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    program->Unbind();
}

void Mesh::DrawWithIndices(const GLenum mode, RWTexture *target) {
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
    if (target != nullptr) target->bindForDraw();
    glDrawElements(mode, size, GL_UNSIGNED_INT, 0);                              CHECK_GL_ERRORS
    if (target != nullptr) target->unbind();
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    program->Unbind();
}

void Mesh::DrawWithIndicesNew(const GLenum mode, Framebuffer *target) {
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
