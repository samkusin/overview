$input v_color0, v_texcoord0

/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include  <bgfx_shader.sh>
#include "shaderlib.sh"

SAMPLER2D(u_texColor, 0);



void main()
{
    vec4 color = texture2D(u_texColor, v_texcoord0);
	gl_FragColor = color * v_color0;
}
