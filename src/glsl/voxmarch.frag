R"(

#version 300 es

precision mediump float;
precision mediump sampler3D;

/* Maximum steps a ray will take through the bounding volume */
#define MAX_STEPS 64
/* Number of voxels per 1.0 unit of space in the world */
#define VOXELS_PER_UNIT 4

/* Bounding volume voxel data (VOXELS_PER_UNIT ^ 3) */
uniform sampler3D voxels;

in vec3 frag_origin;
in vec3 frag_dir;

out vec4 frag_color;

/* Fetch voxel in bounding volume */
float fetch_voxel(vec3 idx) {
    return texture(voxels, idx / float(VOXELS_PER_UNIT)).r;
}

/* 
    Ray to AABB intersection, 
    @returns the near and far intersection distance 
*/
vec2 ray_aabb(in vec3 ro, in vec3 rd, in vec3 boxmin, in vec3 boxmax) {
    vec3 tmin = (boxmin - ro) / rd;
    vec3 tmax = (boxmax - ro) / rd;
	
    vec3 t1 = min(tmin, tmax);
	vec3 t2 = max(tmin, tmax);

	float tN = max( max( t1.x, t1.y ), t1.z );
	float tF = min( min( t2.x, t2.y ), t2.z );
	if( tN > tF || tF < 0.0) return vec2( -1.0, 0.0 );
	
    return vec2( tN, tF );
}

/* Traverse the bounding volume */
bool traverse(vec3 ro, vec3 rd) {
    /* Move up to the edge of the bounding box */
    vec2 bb = ray_aabb(ro, rd, vec3(0.0), vec3(1.0));
    vec3 p = ro + rd * (bb.x + 1e-4/* Small nudge */);
    
    /* Voxel position */
    vec3 vp = p * float(VOXELS_PER_UNIT);
    vec3 idx = floor(vp);
    
    /* Ray direction as fraction */
    vec3 ird = 1. / rd;

    /* Ray direction sign mask */
    vec3 srd = sign(rd);
    vec3 sd = ((idx - vp) + (srd * .5) + .5) * ird;
    
    for (int i = 0; i < MAX_STEPS; ++i) {
        /* Index the voxel data texture */
        float voxel = fetch_voxel(idx);
        
        if (voxel > 0.0) {
            frag_color.rgb = vec3(0.0, voxel, 0.0);
            return true;
        }

        /* Compute the step mask */ 
        vec3 mask = step(sd.xyz, sd.yzx) * step(sd.xyz, sd.zxy);

        /* Step to the next voxel */ 
        sd += mask * srd * ird;
        idx += mask * srd;

        /* Check if we're still within the bounding volume */
        if (any(lessThan(idx, vec3(0))) || any(greaterThanEqual(idx, vec3(VOXELS_PER_UNIT))))
            return false;
    }
    return false;
}

void main() {
    /* Ray origin and direction */ 
	vec3 ray_dir = normalize(frag_dir);
	vec3 ray_origin = frag_origin;
    
    /* Traverse through the bounding volume */ 
    if (traverse(ray_origin, ray_dir) == false) {
        discard;
    }
}

)"