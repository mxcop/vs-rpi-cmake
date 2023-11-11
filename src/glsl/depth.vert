R"(

#version 300 es
layout (location = 0) in vec3 pos;

const vec2 madd=vec2(0.5,0.5);
out vec2 texcoord;

void main()
{
	texcoord = pos.xy*madd+madd;
    gl_Position = vec4(pos, 1.0);
}

)"