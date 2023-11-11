R"(

#version 300 es
layout (location = 0) in vec3 pos;

/* Model matrix, for transforming the mesh in the world */
uniform mat4 model;
/* Projection matrix, for projecting onto the screen */
uniform mat4 projection;

void main()
{
    gl_Position = projection * model * vec4(pos, 1.0);
}

)"