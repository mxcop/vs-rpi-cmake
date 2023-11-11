R"(

#version 300 es
precision mediump float;

in vec3 frag_origin;
in vec3 frag_dir;

out vec4 frag_color;

#define EPSILON 0.001
#define NUM_STEPS 32

float sphere_sdf(vec3 pos, vec3 sphere_pos, float r)
{
	return length(pos - sphere_pos) - r;
}

float eval_scene(vec3 pos)
{
	return sphere_sdf(pos, vec3(0, 0, 0), 1.0);
}

void main ()
{
	vec3 dir = normalize(frag_dir);
	vec3 point = frag_origin;

	for (int i = 0; i < NUM_STEPS; i++)
	{
		float dist = eval_scene(point);

		if (dist < EPSILON)
		{
			frag_color = vec4(point.xyz, 1);
            gl_FragDepth = gl_FragCoord.z; // TODO: use actual depth
			return;
		}

		point += dir * dist;
	}

	frag_color = vec4(0.5, 0.5, 0.5, 1);
    gl_FragDepth = 1.0;
}

)"