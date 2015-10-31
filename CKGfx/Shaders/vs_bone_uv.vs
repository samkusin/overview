$input a_position, a_normal, a_texcoord0, a_indices, a_weight
$output v_normal, v_texcoord0, v_position

/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include  <bgfx_shader.sh>
#include "shaderlib.sh"
#include "ckgfx.sh"

/* The u_model array will contain bone transforms. */

uniform mat4 u_world;
uniform mat4 u_worldViewProj;

void main()
{
    ivec4 indices = ivec4(a_indices);
    
    mat4 boneMat = mul(u_model[int(indices.x)],a_weight.x);
    boneMat += mul(u_model[int(indices.y)],a_weight.y);
    boneMat += mul(u_model[int(indices.z)],a_weight.z);
    boneMat += mul(u_model[int(indices.w)],a_weight.w);
        
    vec4 bpos = mul(boneMat, vec4(a_position, 1.0));
    gl_Position = mul(u_worldViewProj, bpos );
    v_texcoord0 = a_texcoord0;
    
    vec4 bnorm = mul(boneMat, vec4(a_normal, 0.0));
    v_normal = mul(u_world, bnorm).xyz;
    v_position = mul(u_world, bpos).xyz;
}
