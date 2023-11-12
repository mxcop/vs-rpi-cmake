R"(

#version 300 es
precision mediump float;
precision mediump sampler3D;

// uniform vec3 world_pos;
uniform sampler3D voxels;

in vec3 frag_origin;
in vec3 frag_dir;

out vec4 frag_color;

#define MAX_STEPS 64
#define VOXELS_PER_UNIT 4

// float sphere_sdf(vec3 pos, vec3 sphere_pos, float r)
// {
// 	return length(pos - sphere_pos) - r;
// }

/* Check for voxel in world */
bool get_voxel(vec3 c) {
    return texture(voxels, c / float(VOXELS_PER_UNIT)).r > 0.0;
    //return texture(voxels, vec3(0.0)).r > 0.0;

    //vec3 p = c + vec3(0.5 / VOXELS_PER_UNIT);
    //float d = sphere_sdf(p, vec3(0.0), 1.0);
    //return d < 0.0;
}

vec2 boxIntersection( in vec3 ro, in vec3 rd, in vec3 boxmin, in vec3 boxmax ) 
{
    vec3 tmin = (boxmin - ro) / rd;
    vec3 tmax = (boxmax - ro) / rd;
	
    vec3 t1 = min(tmin, tmax);
	vec3 t2 = max(tmin, tmax);

	float tN = max( max( t1.x, t1.y ), t1.z );
	float tF = min( min( t2.x, t2.y ), t2.z );
	if( tN > tF || tF < 0.0) return vec2( -1.0, 0.0 );
	
    return vec2( tN, tF );
}

float dist_intersect_aabb( in vec3 ro, in vec3 rd, in vec3 boxmin, in vec3 boxmax) 
{
    vec3 tmin = (boxmin - ro) / rd;
    vec3 tmax = (boxmax - ro) / rd;
	
    vec3 t1 = min(tmin, tmax);
	vec3 t2 = max(tmin, tmax);

	float tN = max( max( t1.x, t1.y ), t1.z );
	float tF = min( min( t2.x, t2.y ), t2.z );
	if( tN > tF || tF < 0.0) return -1.0;
	
	return tN;
}

void traversal(vec3 ro, vec3 rd) {
    // vec3 bn;
    vec2 bb = boxIntersection(ro, rd, vec3(0.0), vec3(1.0));
    
    vec3 p = ro + rd * (bb.x + 1e-4);
    
    // vec3 vp = (p + .5) * float(VOXELS_PER_UNIT);
    vec3 vp = p * float(VOXELS_PER_UNIT);
    vec3 id = floor(vp);
    vec3 ird = 1. / rd;
    vec3 srd = sign(rd);
    vec3 dd = abs(ird);
    vec3 sd = ((id - vp) + (srd * .5) + .5) * ird;
    
    vec3 m;
    bool hit = false;
    
    vec3 col;
    
    if (bb.x > 0.)
    {
        for (int i = 0; i < MAX_STEPS; ++i)
        {
            //#if 0
            //bool b = length(id - float(VOXELS_PER_UNIT) * .5) < float(VOXELS_PER_UNIT) * .5;
            //#else
            float b = texture(voxels, id / float(VOXELS_PER_UNIT)).r;
            //#endif
            
            if (b > 0.0)
            {
                frag_color.rgb = vec3(0.0, b, 0.0);
                return;
            }

            m = step(sd.xyz, sd.yzx) * step(sd.xyz, sd.zxy);

            sd += m * srd * ird;
            id += m * srd;

            // if (any(lessThan(id, vec3(0))) || any(greaterThanEqual(id, vec3(VOXELS_PER_UNIT))))
            //     break;
        }

        // m = any(bvec3(m)) ? m : abs(bn);
        
        // vec3 n = -m * srd;

        // vec3 ldir = cmat * normalize(vec3(2., 1., .5));
        
        // vec3 alb = palette(hash13(id));
        // vec3 dif = vec3(1);
        // vec3 gl = getEnvironmentMap(reflect(rd, n));
        // float fr = pow(max(dot(-rd, n), 0.), 5.);

        // col = mix(dif, gl, 1.-fr) * alb;
        // col += max(dot(n, ldir), 0.) * alb;
        // col *= dot(m, vec3(.8, 1., .6));
        // col *= float(hit);

        frag_color.rgb = vec3(0.5, 0.0, 0.0);
        return;
    }

    frag_color.rgb = vec3(0.0, 0.0, 0.5);
}

void traverse(vec3 ro, vec3 rd) {
    ro += rd * dist_intersect_aabb(ro, rd, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
    
    vec3 vox_idx = floor(ro * float(VOXELS_PER_UNIT));
    // vec3 ray_pos = floor(ro * VOXELS_PER_UNIT);
    vec3 ray_dir_inv = 1.0 / rd;
    vec3 ray_step = sign(rd);
	vec3 ray_delta = min(ray_dir_inv * ray_step, 1.0);

    // start at intersection of ray with initial cell
    // vec3 t_max = ray_pos;// abs((ray_pos + max(ray_step, vec3(0.0)) - ro) * ray_dir_inv);
    // vec3 t_max = (sign(rd) * (floor(ro) - ro) + (sign(rd) * 0.5) + 0.5) * ray_delta;
    vec3 t = abs((ro + max(ray_step, vec3(0.0)) - ro) * ray_dir_inv);
    float voxel = 0.0;

    for (int i = 0; i < MAX_STEPS; ++i) {
        if (voxel > 0.0) {
            frag_color.rgb = vec3(0.0, voxel, 0.0);
            return;
        }

        vec3 t_min = vec3(min(min(t.x,t.y),t.z));
        vec3 incr = vec3(equal(t, t_min));

        t += ray_delta * incr;
        vox_idx += ray_step * incr;

        voxel = texture(voxels, vox_idx).r;

        // if (next_t > 1.0) break;
        // vec3 idx = floor(ray_pos) / VOXELS_PER_UNIT;

        // if (texture(voxels, idx).r > 0.0) {
        //     frag_color.rgb = vec3(0.0, texture(voxels, idx).r, 0.0);
        //     break;
        // }
        
	    // set_source_rgb(vec3(0.0));
        // circle(p0 + next_t * rd, 0.15);
        // fill();
        
        // vec3 cmp = vec3(lessThanEqual(t_max.xyz, min(t_max.yzx, t_max.zxy)));// step(t_max.xyz, t_max.xyz);
        // t_max += ray_delta * cmp;
        // ray_pos += ray_step * cmp;
    }    

    // while voxel is empty & inside the grid
    // while ( voxel == 0 )
    // {
    //     // find minimum parameter distance
    //     tmin = min(t.x, min(t.y,t.z));
    //     // determine the step direction
    //     incr.xyz = (t.xyz == tmin.xxx);
    //     // advance
    //     t.xyz += tStep*incr;
    //     voxelIdx.xyz += cellStep *incr;
    //     // read next voxel
    //     voxel = tex3D(texAccelGrid, voxelIdx*invGridSize).a;
    // }
}

void main()
{
    /* Ray origin and direction */ 
	vec3 ray_dir = normalize(frag_dir);
	vec3 ray_origin = frag_origin;

    /* Move up to the edge of the bounding volume */
    // ray_origin += ray_dir * (dist_intersect_aabb(frag_origin, ray_dir, vec3(-1.0, -1.0, -1.0), vec3(1.0, 1.0, 1.0)) - 0.1);
    
    /* Convert the ray position to the voxel grid */ 
	ivec3 map_pos = ivec3(ray_origin * float(VOXELS_PER_UNIT));

    vec3 delta_dist = abs(vec3(length(ray_dir)) / ray_dir);
    ivec3 ray_step = ivec3(sign(ray_dir));
    vec3 side_dist = (sign(ray_dir) * (floor(ray_origin) - ray_origin) + (sign(ray_dir) * 0.5) + 0.5) * delta_dist; 
    
    bvec3 mask = bvec3(0);
    
    /* Step through the bounding volume */ 
    // frag_color.rgb = vec3(0.5);
    traversal(ray_origin, ray_dir);
    // for (int i = 0; i < MAX_STEPS; i++) {
    //     mask = lessThanEqual(side_dist.xyz, min(side_dist.yzx, side_dist.zxy));

    //     if (get_voxel(map_pos)) {
    //         frag_color.rgb = vec3(mask.xyz);
    //         return;
    //     }

    //     side_dist += vec3(mask) * delta_dist;
    //     map_pos += ivec3(mask) * ray_step;
    // }
    
    
    //discard;
}

)"