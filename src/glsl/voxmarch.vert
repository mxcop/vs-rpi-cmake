R"(

#version 300 es
layout (location = 0) in vec3 pos;

uniform vec3 camera_pos;
uniform mat4 inverse_model;

// TODO: combine model, view, and projection matrices into one called "mvp"
/* Model matrix, for transforming the mesh in the world */
uniform mat4 model;
/* View matrix, for moving the world around the camera */
uniform mat4 view;
/* Projection matrix, for projecting onto the screen */
uniform mat4 projection;

out vec3 frag_origin;
out vec3 frag_dir;

void main()
{
    // texcoord = projection * view * model * vec4(pos.xy*madd+madd, 1.0, 1.0);
    gl_Position = projection * view * model * vec4(pos, 1.0);

    // Convert camera position from world-space to local-space of the model
	vec3 camera_local = (inverse_model * vec4(camera_pos, 1.0)).xyz;

	// Ray origin and direction
	frag_origin = camera_local;
	frag_dir = (pos - camera_local);
}

)"