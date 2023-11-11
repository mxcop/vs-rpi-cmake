#pragma once

#include <glad/glad.h>

/**
 * @brief [OpenGL] Mesh wrapper.
 */
struct Mesh {
    /* [OpenGL] Vertex buffer object handle */
    GLuint vbo_handle = 0;
    /* [OpenGL] Vertex array object handle */
    GLuint vao_handle = 0;
    int indices = 0;

    Mesh() = default;
    /**
     * @param vertices Vertex points of the mesh as triangles.
     * @param size Size of the vertex array in bytes.
     */
    Mesh(const GLfloat* vertices, int size);
    ~Mesh();

    /* Not move-able */
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    void use() const;
    void draw() const;
};
