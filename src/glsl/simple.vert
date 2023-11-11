R"(

#version 300 es
layout (location = 0) in vec3 pos;

/* Model matrix, for transforming the mesh in the world */
uniform mat4 model;
/* View matrix, for moving the world around the camera */
uniform mat4 view;
/* Projection matrix, for projecting onto the screen */
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
}

)"