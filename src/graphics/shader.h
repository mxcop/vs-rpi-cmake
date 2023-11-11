#pragma once

#include <glm/fwd.hpp>
#include <glad/glad.h>

/**
 * @brief [OpenGL] Shader wrapper.
 */
struct Shader {
    /* [OpenGL] Shader object handle */
    GLuint handle = 0;

    Shader() = default;
    /**
     * @param vert_src Vertex shader source code.
     * @param frag_src Fragment shader source code.
     */
    Shader(const char* vert_src, const char* frag_src);
    ~Shader();

    /* Not move-able */
    Shader(Shader&&) = delete;
    Shader& operator=(Shader&&) = delete;

    /**
     * @brief Start using this shader program.
     */
    void use() const;

    void set_float(const char* name, float value) const;
    void set_int(const char* name, int value) const;
    void set_vec2f(const char* name, const glm::vec2& vec) const;
    void set_vec3f(const char* name, const glm::vec3& vec) const;
    void set_mat4(const char* name, const glm::mat4& matrix) const;
};
