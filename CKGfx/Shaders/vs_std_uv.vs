$input a_position, a_normal, a_texcoord0
$output v_normal, v_texcoord0, v_position

/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include  <bgfx_shader.sh>
#include "shaderlib.sh"
#include "ckgfx.sh"

void main()
{
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
    v_texcoord0 = a_texcoord0;
    v_normal = mul(u_model[0],vec4(a_normal, 0.0)).xyz;
    v_position = mul(u_model[0], vec4(a_position, 1.0)).xyz;
}
