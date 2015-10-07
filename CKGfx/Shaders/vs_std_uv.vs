$input a_position, a_normal, a_texcoord0
$output v_color0, v_texcoord0

/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include  <bgfx_shader.sh>
#include "shaderlib.sh"

void main()
{
    //vec3 lightPos = mul(u_modelView, vec4(0.0, 1.0, -5.0, 1.0)).xyz;
    //vec3 lightPos = vec3(0.0, 5.0, -5.0);
    //vec3 normal = mul(u_modelView, vec4(a_normal, 0.0)).xyz;
    //vec3 eyePos = mul(u_modelView, vec4(a_position, 1.0)).xyz;
    //vec3 s = normalize(lightPos - eyePos);
    
    //vec3 Kd = vec3(1.0, 1.0, 1.0);  // diffuse reflectivity
    //vec3 Ld = vec3(1.0, 1.0, 1.0);  // diffuse color
    //vec3 light = mul(mul(Ld, Kd), max(dot(s, normal), 0.0));
    
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
	//v_color0 = vec4(light, 1.0);
    v_color0 = vec4(1.0, 1.0, 1.0, 1.0);
    v_texcoord0 = a_texcoord0;
}
