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

float intersect_aabb(in vec3 ro, in vec3 rd, in vec3 boxmin, in vec3 boxmax) {
    vec3 tMin = (boxmin - ro) / rd;
	vec3 tMax = (boxmax - ro) / rd;

	vec3 t1 = min(tMin, tMax);
	// vec3 t2 = max(tMin, tMax);

	float tNear = max(max(t1.x, t1.y), t1.z);
	// float tFar = min(min(t2.x, t2.y), t2.z);

	return max(tNear, 0.0);
}

void main ()
{
	vec3 dir = normalize(frag_dir);
	vec3 point = frag_origin;

    /* Discard every other pixel */
    // if (mod(gl_FragCoord.x, 2.0) < 1.0 || mod(gl_FragCoord.y, 2.0) < 1.0) {
    //     discard;
    // }

    /* Move into the bounding box */
    // point += dir * intersect_aabb(point, dir, vec3(-1.0), vec3(1.0)); 

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
	// discard;
}

)"