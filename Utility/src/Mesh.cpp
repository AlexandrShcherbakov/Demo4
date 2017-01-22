#include "Mesh.h"

namespace GL {

Mesh::Mesh() {
    glGenVertexArrays(1, &ID);                                                   CHECK_GL_ERRORS
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &ID);                                                CHECK_GL_ERRORS
}

void Mesh::Draw(const uint count, RWTexture *target) {
    for (auto &tex: textures)
        tex.second->bindTexture(*program, tex.first);
    for (auto &light: spots)
        program->setLight(light.first, *(light.second));
    for (auto &light: directionals)
        program->setLight(light.first, *(light.second));
    program->setCamera("camera", *camera);
    glUseProgram(program->ID);                                                   CHECK_GL_ERRORS
    glBindVertexArray(ID);                                                       CHECK_GL_ERRORS
    if (target != nullptr) target->bindForDraw();
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, count);                                        CHECK_GL_ERRORS
    if (target != nullptr) target->unbind();
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

void Mesh::DrawWithIndices(const GLenum mode, RWTexture *target) {
    for (auto &tex: textures) {
        tex.second->bindTexture(*program, tex.first);
    }
    for (auto &light: spots)
        program->setLight(light.first, *(light.second));
    for (auto &light: directionals)
        program->setLight(light.first, *(light.second));
    program->setCamera("camera", *camera);
	material.bindMaterial(*program);
    glUseProgram(program->ID);                                                   CHECK_GL_ERRORS
    glBindVertexArray(ID);                                                       CHECK_GL_ERRORS
    if (target != nullptr) target->bindForDraw();
    glDrawElements(mode, size, GL_UNSIGNED_INT, 0);                              CHECK_GL_ERRORS
    if (target != nullptr) target->unbind();
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}


}
