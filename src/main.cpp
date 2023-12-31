﻿#include <stdio.h>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/shader.h"
#include "graphics/mesh.h"

#define DB_DEPTH 0

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_position_callback(GLFWwindow* window, double x, double y);
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods);
void input_thread(GLFWwindow* window);

constexpr GLfloat g_vertex_buffer_data[] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // triangle 1.
    1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // triangle 2.
    1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // triangle 3.
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // triangle 4.
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // triangle 5.
    1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // triangle 6.
    0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,  // triangle 7.
    1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // triangle 8.
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  // triangle 9.
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // triangle 10.
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // triangle 11.
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f   // triangle 12.
};

constexpr float quad[] = {-1.0f, -1.0f, 0.0f, 1.0f,  -1.0f, 0.0f,
                          -1.0f, 1.0f,  0.0f, -1.0f, 1.0f,  0.0f,
                          1.0f,  -1.0f, 0.0f, 1.0f,  1.0f,  0.0f};

#define VOXELS_PER_UNIT 16
#define VOXELS VOXELS_PER_UNIT* VOXELS_PER_UNIT* VOXELS_PER_UNIT

unsigned char voxel_data[VOXELS] = {0u};

int view_width, view_height;
glm::vec2 mouse_delta;
glm::vec3 camera_pos;
glm::vec3 camera_forward;
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
bool w, a, s, d, up, down;

void error_callback(int code, const char* description) {
    printf("error glfw: code %i\n%s\n", code, description);
}

int main(int argc, char* argv[]) {
    glfwSetErrorCallback(error_callback);

    // Instantiate the GLFW Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Creating the Window Object
    view_width = 800;
    view_height = 600;
    GLFWwindow* window = glfwCreateWindow(800, 600, "RPi OpenGL", NULL, NULL);
    if (window == NULL) {
        printf("failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glfwSwapInterval(1 /* vsync */);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("failed to initialize GLAD\n");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set Viewport Dimensions
    glViewport(0, 0, 800, 600);

    /* Voxel data texture */
    for (int i = 0; i < VOXELS; i++) {
        if (rand() % 5 == 0)
            voxel_data[i] = rand() % 256;
        else
            voxel_data[i] = 0u;
    }

    // voxel_data[0] = 1u;
    // voxel_data[31] = 1u;
    // voxel_data[63] = 1u;

    GLuint voxel_tex;

    glGenTextures(1, &voxel_tex);
    glBindTexture(GL_TEXTURE_3D, voxel_tex);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, VOXELS_PER_UNIT, VOXELS_PER_UNIT,
                 VOXELS_PER_UNIT, 0, GL_RED, GL_UNSIGNED_BYTE, voxel_data);
    // glGenerateMipmap(GL_TEXTURE_3D);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, voxel_tex);

    /* Init buffers for triangle */
    Mesh triangle = Mesh(g_vertex_buffer_data, sizeof(g_vertex_buffer_data));

    /* Load & compile the shader */
    const char* vert_src =
#include "glsl/voxmarch.vert"
        ;
    const char* frag_src =
#include "glsl/voxmarch.frag"
        ;
    Shader shader = Shader(vert_src, frag_src);

    /* Bind shader program & vertex array */
    shader.use();
    triangle.use();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

#if DB_DEPTH
    Mesh screen = Mesh(quad, sizeof(quad));

    const char* d_vert_src =
#include "glsl/depth.vert"
        ;
    const char* d_frag_src =
#include "glsl/depth.frag"
        ;
    Shader depth_shader = Shader(d_vert_src, d_frag_src);

    GLuint depth_tex;
    glGenTextures(1, &depth_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
#endif

    float yaw = 0.0f;
    float pitch = 0.0f;

    /* Start input thread */
    std::thread input_th(input_thread, window);

    constexpr int MW = 32, MH = 32, MD = 32;

    static glm::mat4 models[MW][MH][MD] = {};
    static glm::mat4 models_inv[MW][MH][MD] = {};
    {
        glm::mat4 model = glm::mat4(1.0f /* Identity matrix */);
        for (int y = 0; y < MH; y++) {
            for (int x = 0; x < MW; x++) {
                for (size_t z = 0; z < MD; z++) {
                    models[x][y][z] = glm::translate(
                        model,
                        glm::vec3(5.0f + x * 2.0f, z * 2.0f, 5.0f + y * 2.0f));
                    models_inv[x][y][z] = glm::inverse(models[x][y][z]);
                }
            }
        }
    }

    double prev_time = glfwGetTime();
    double time = 0.0;
    /* Rendering loop */
    while (!glfwWindowShouldClose(window)) {
        double dt = glfwGetTime() - prev_time;
        prev_time = glfwGetTime();
        time += dt;

        /* Rotate camera */
        yaw += mouse_delta.x * dt * 2.0f;
        pitch -= mouse_delta.y * dt * 2.0f;
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        glm::vec3 direction;
        float xzLen = cos(glm::radians(pitch));
        direction.x = xzLen * cos(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        direction.z = xzLen * sin(glm::radians(yaw));
        camera_forward = glm::normalize(direction);

        /* Only draw at 65 fps */
        if (time < 1.0 / 65.0 /* fps */) continue;
        time = 0.0;

        /* Clear the screen */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Update the view and projection matrices */
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(camera_pos, camera_pos + camera_forward, camera_up);
        shader.set_mat4("view", view);
        mouse_delta = glm::vec2(0.0f);

        shader.set_mat4("projection",
                        glm::perspective(
                            80.0f, static_cast<float>(view_width) / view_height,
                            0.1f, 100.0f));
        shader.set_vec3f("camera_pos", camera_pos);

        for (int y = 0; y < MH; y++) {
            for (int x = 0; x < MW; x++) {
                for (size_t z = 0; z < MD; z++) {
                    shader.set_mat4("model", models[x][y][z]);
                    shader.set_mat4("inverse_model", models_inv[x][y][z]);

                    /* Draw triangle */
                    triangle.draw();
                }
            }
        }

#if DB_DEPTH
        depth_shader.use();
        screen.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depth_tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, view_width,
                     view_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, view_width,
                            view_height);

        glDisable(GL_DEPTH_TEST);
        screen.draw();
        glEnable(GL_DEPTH_TEST);

        shader.use();
        triangle.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, voxel_tex);
#endif

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
    view_width = width;
    view_height = height;
}

void input_thread(GLFWwindow* window) {
    double prev_time = glfwGetTime();
    /* Input Event Loop */
    while (!glfwWindowShouldClose(window)) {
        double d_dt = glfwGetTime() - prev_time;
        
        /* Avoid delta time which is too small for float precision */
        if (d_dt < 1.0f / 10000.0f) {
            continue;
        }
        float dt = static_cast<float>(d_dt);
        prev_time = glfwGetTime();

        /* Exit on escape */
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        /* Move the camera */
        if (w) camera_pos += camera_forward * dt * 8.0f;
        if (s) camera_pos -= camera_forward * dt * 8.0f;
        if (a)
            camera_pos +=
                glm::normalize(glm::cross(camera_forward, camera_up)) * dt *
                8.0f;
        if (d)
            camera_pos -=
                glm::normalize(glm::cross(camera_forward, camera_up)) * dt *
                8.0f;
        if (up) camera_pos -= camera_up * dt * 8.0f;
        if (down) camera_pos += camera_up * dt * 8.0f;
    }
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
    static double ox = 0, oy = 0;  // TODO: remove statics
    mouse_delta = glm::vec2(ox - x, oy - y);
    ox = x, oy = y;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {
    /* Questionable input code... */
    if (key == GLFW_KEY_W && action == GLFW_PRESS) w = true;
    if (key == GLFW_KEY_W && action == GLFW_RELEASE) w = false;

    if (key == GLFW_KEY_A && action == GLFW_PRESS) a = true;
    if (key == GLFW_KEY_A && action == GLFW_RELEASE) a = false;

    if (key == GLFW_KEY_S && action == GLFW_PRESS) s = true;
    if (key == GLFW_KEY_S && action == GLFW_RELEASE) s = false;

    if (key == GLFW_KEY_D && action == GLFW_PRESS) d = true;
    if (key == GLFW_KEY_D && action == GLFW_RELEASE) d = false;

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) up = true;
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) up = false;

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) down = true;
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) down = false;
}
