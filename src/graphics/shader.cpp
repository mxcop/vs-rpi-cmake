#include "shader.h"

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> /* "glm::value_ptr" */

/* DEBUG safety check */
#ifdef _DEBUG
#include <stdio.h>
#define INIT_GUARD                                                   \
    if (handle == NULL) {                                            \
        printf(                                                      \
            "error: in %s at line %d:\nattempt to call function on " \
            "uninitialized shader.\n",                               \
            __FILE__, __LINE__);                                     \
        abort();                                                     \
    }
#else
#define INIT_GUARD ;
#endif

/**
 * @brief Print any shader compilation errors to the console.
 */
bool log_err(unsigned int shader_id, std::string type) {
    int success;
    char info_log[1024];
    if (type != "shader program") {
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader_id, 1024, NULL, info_log);
            printf("compile-time error: in %s\n%s", type.c_str(), info_log);
        }
    } else {
        glGetProgramiv(shader_id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader_id, 1024, NULL, info_log);
            printf("link-time error: in %s\n%s", type.c_str(), info_log);
        }
    }
    return success;
}

/**
 * @brief Compile shaders, combine them into a program, and store it on the
 * GPU.
 *
 * @param vert_src Vertex shader source code.
 * @param frag_src Fragment shader source code.
 * @return The handle of the shader program, if an error occured the handle will
 * be zero.
 */
GLuint compile(const char* vert_src, const char* frag_src) {
    /* Don't compile if either the vertex or fragment shader is null */
    if (vert_src == nullptr || frag_src == nullptr) return 0;

    GLuint vert_shader, frag_shader;

    /* Compile vertex shader */
    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vert_src, NULL);
    glCompileShader(vert_shader);
    if (!log_err(vert_shader, "vertex shader")) {
        /* Cleanup if compiling failed */
        glDeleteShader(vert_shader);
        return 0;
    }

    /* Compile fragment shader */
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &frag_src, NULL);
    glCompileShader(frag_shader);
    if (!log_err(frag_shader, "fragment shader")) {
        /* Cleanup if compiling failed */
        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);
        return 0;
    }

    /* Create the shader program */
    GLuint handle = glCreateProgram();

    /* Attach the compiled shaders */
    glAttachShader(handle, vert_shader);
    glAttachShader(handle, frag_shader);

    /* Link the shader program */
    glLinkProgram(handle);
    if (!log_err(handle, "shader program")) {
        /* Cleanup if linking failed */
        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);
        glDeleteProgram(handle);
        return 0;
    }

    /*
        Finally, delete the shaders once linked.
        Now they're stored on the GPU.
    */
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    return handle;
}

Shader::Shader(const char* vert_src, const char* frag_src)
    : handle(compile(vert_src, frag_src)) {}

Shader::~Shader() {
    INIT_GUARD
    glDeleteProgram(handle);
}

void Shader::use() const {
    INIT_GUARD
    glUseProgram(handle);
}

void Shader::set_float(const char* name, float value) const {
    INIT_GUARD
    glUniform1f(glGetUniformLocation(handle, name), value);
}

void Shader::set_int(const char* name, int value) const {
    INIT_GUARD
    glUniform1i(glGetUniformLocation(handle, name), value);
}

void Shader::set_vec2f(const char* name, const glm::vec2& vec) const {
    INIT_GUARD
    glUniform2f(glGetUniformLocation(handle, name), vec.x, vec.y);
}

void Shader::set_mat4(const char* name, const glm::mat4& matrix) const {
    INIT_GUARD
    glUniformMatrix4fv(glGetUniformLocation(handle, name), 1, false,
                       glm::value_ptr(matrix));
}
