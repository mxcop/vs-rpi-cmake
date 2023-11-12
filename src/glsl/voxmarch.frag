R"(

#version 300 es

precision mediump float;
precision mediump sampler3D;

/* Maximum steps a ray will take through the bounding volume */
#define MAX_STEPS 64
/* Number of voxels per 1.0 unit of space in the world */
#define VOXELS_PER_UNIT 16

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
    @returns the distance to the nearest intersection.
*/
float ray_aabb(in vec3 ro, in vec3 ird, in vec3 boxmin, in vec3 boxmax) {
    vec3 tmin = ird * (boxmin - ro);
    vec3 tmax = ird * (boxmax - ro);
	
    vec3 t1 = min(tmin, tmax);
	float tN = max( max( t1.x, t1.y ), t1.z );
	
    return max(tN, 0.0);
}

/* Traverse the bounding volume */
bool traverse(in vec3 ro, in vec3 rd, in vec3 ird) {
    /* Move up to the edge of the bounding box */
    float bb = ray_aabb(ro, ird, vec3(0.0), vec3(1.0));
    vec3 p = ro + rd * (bb + 1e-4/* Small nudge */);
    
    /* Voxel position */
    vec3 vp = p * float(VOXELS_PER_UNIT);
    vec3 idx = floor(vp);
    
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
    /* Ray origin, direction, and direction as fraction */ 
	vec3 ray_dir = normalize(frag_dir);
	vec3 ray_invdir = 1. / ray_dir;
	vec3 ray_origin = frag_origin;
    
    /* Traverse through the bounding volume */ 
    if (traverse(ray_origin, ray_dir, ray_invdir) == false) {
        discard;
    }
}

)"