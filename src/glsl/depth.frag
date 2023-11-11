R"(

#version 300 es
precision mediump float;
out vec4 frag_color;
in vec2 texcoord;

uniform sampler2D depth_buffer;

float lindepth(in vec2 uv)
{
    float zNear = 1.0;   
    float zFar  = 100.0;
    float depth = texture2D(depth_buffer, uv).x;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
    float c = lindepth(texcoord);
    frag_color = vec4(c, c, c, 1.0);
}

)"