$input v_color0, v_texcoord0

/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include  <bgfx_shader.sh>
#include "shaderlib.sh"

SAMPLER2D(u_tex0, 0);

void main()
{    
    vec4 color = texture2D(u_tex0, v_texcoord0);
    if (color.a < 0.1)
        discard;
    gl_FragColor = mul(color,v_color0);
}
