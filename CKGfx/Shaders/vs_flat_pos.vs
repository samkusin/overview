$input a_position
$output v_color0

/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include  <bgfx_shader.sh>
#include "shaderlib.sh"
#include "ckgfx.sh"

uniform vec4 u_color;

void main()
{
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
    v_color0 = u_color;
}
