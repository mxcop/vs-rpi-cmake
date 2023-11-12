#include "mesh.h"

Mesh::Mesh(const GLfloat* vertices, int size) {
    /* Generate and bind buffer */
    glGenVertexArrays(1, &vao_handle);
    glBindVertexArray(vao_handle);

    /* Generate and bind buffer */
    glGenBuffers(1, &vbo_handle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);

    /* Upload buffer data */
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    /* Position attribute */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    /* Unbind buffer */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    indices = size / sizeof(GLfloat);
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &vbo_handle);
    glDeleteVertexArrays(1, &vao_handle);
}

void Mesh::use() const {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
    glBindVertexArray(vao_handle);
}

void Mesh::draw() const {
    glDrawArrays(GL_TRIANGLES, 0, indices / 3);
}
