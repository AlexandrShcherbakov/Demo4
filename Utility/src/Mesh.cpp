#include "Mesh.h"

namespace GL {

Mesh::Mesh() {
    glGenVertexArrays(1, &ID);                                                   CHECK_GL_ERRORS
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &ID);                                                CHECK_GL_ERRORS
}

void Mesh::bindBuffer(const Buffer& buf, const uint index) {
    glBindVertexArray(ID);                                                       CHECK_GL_ERRORS
	buf.bind();
	uint components = buf.getComponents();
    if (buf.isFloat())
		glVertexAttribPointer(index, components, buf.getElementsType(), GL_FALSE, 0, 0);  CHECK_GL_ERRORS
	if (buf.isInt())
		glVertexAttribIPointer(index, components, buf.getElementsType(), 0, 0);  CHECK_GL_ERRORS
	if (buf.isDouble())
		glVertexAttribLPointer(index, components, buf.getElementsType(), 0, 0);  CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
}

void Mesh::bindBuffer(const Buffer& buf, ShaderProgram prog, const std::string& name) {
	glBindVertexArray(ID);                                                       CHECK_GL_ERRORS
	buf.bind();
	GLuint index = glGetAttribLocation(prog.ID, name.c_str());                   CHECK_GL_ERRORS
    glEnableVertexAttribArray(index);                                            CHECK_GL_ERRORS
    uint components = buf.getComponents();
    if (buf.isFloat())
		glVertexAttribPointer(index, components, buf.getElementsType(), GL_FALSE, 0, 0);  CHECK_GL_ERRORS
	if (buf.isInt())
		glVertexAttribIPointer(index, components, buf.getElementsType(), 0, 0);  CHECK_GL_ERRORS
	if (buf.isDouble())
		glVertexAttribLPointer(index, components, buf.getElementsType(), 0, 0);  CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
}

void Mesh::bindIndicesBuffer(const Buffer& buf) {
    glBindVertexArray(ID);                                                       CHECK_GL_ERRORS
	buf.bind();
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    this->size = buf.getSize();
}

void Mesh::Draw(const uint count, RWTexture *target) {
    for (auto &tex: textures)
        tex.second->bindTexture(*program, tex.first);
    for (auto &light: lights)
        program->setLight(light.first, *(light.second));
    program->setCamera("camera", *camera);
    glUseProgram(program->ID);                                                   CHECK_GL_ERRORS
    glBindVertexArray(ID);                                                       CHECK_GL_ERRORS
    if (target != nullptr) target->bindForDraw();
    glDrawArrays(GL_TRIANGLES, 0, count);                                        CHECK_GL_ERRORS
    if (target != nullptr) target->unbind();
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

void Mesh::DrawWithIndices(RWTexture *target) {
    for (auto &tex: textures) {
        tex.second->bindTexture(*program, tex.first);
    }
    for (auto &light: lights)
        program->setLight(light.first, *(light.second));
    program->setCamera("camera", *camera);
    //program->setCamera("camera", *(lights.begin()->second));
	material.bindMaterial(*program);
    glUseProgram(program->ID);                                                   CHECK_GL_ERRORS
    glBindVertexArray(ID);                                                       CHECK_GL_ERRORS
    if (target != nullptr) target->bindForDraw();
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);                      CHECK_GL_ERRORS
    if (target != nullptr) target->unbind();
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}


}
