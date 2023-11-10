#include <stdio.h>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "graphics/shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void input_thread(GLFWwindow* window);

constexpr float vertices[] = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f,
                              0.0f,  0.0f,  0.5f, 0.0f };

void error_callback(int code, const char* description)
{
    printf("error glfw: code %i\n%s\n", code, description);
}

int main(int argc, char* argv[]) {
    glfwSetErrorCallback(error_callback);

    // Instantiate the GLFW Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Creating the Window Object
    GLFWwindow* window = glfwCreateWindow(800, 600, "RPi OpenGL", NULL, NULL);
    if (window == NULL) {
        printf("failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1/* vsync */);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("failed to initialize GLAD\n");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Set Viewport Dimensions
    glViewport(0, 0, 800, 600);

    /* Init buffers for triangle */
    unsigned int vbo, vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    /* Unbind triangle buffers */
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Load & compile the shader */
    const char* vert_src =
#include "glsl/simple.vert"
        ;
    const char* frag_src =
#include "glsl/simple.frag"
        ;
    Shader shader = Shader(vert_src, frag_src);

    /* Bind shader program & vertex array */
    shader.use();
    glBindVertexArray(vao);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    /* Start input thread */
    std::thread input_th(input_thread, window);

    /* Rendering loop */
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        /* Draw triangle */
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents(); /* <- needs to be on the main thread! */
    }

    /* Join input thread */
    input_th.join();

    /* Cleanup */
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void input_thread(GLFWwindow* window) {
    /* Input Event Loop */
    while (!glfwWindowShouldClose(window)) {
        /* Exit on escape */
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }
}
