$input a_position, a_color0, a_texcoord0
$output v_color0, v_texcoord0

/*
 * Copyright 2015 Cinekine Ltd
 * License: http://opensource.org/licenses/MIT
 * 
 * Vertex Shader
 * Platform(s): GL
 *
 * Platform Notes GL:
 *     gl_VertexID used - note, GLES2 does not support this, but GLES3 does.
 *
 *
 * Uniforms:
 *      u_scale - size of quad/2
 */

#include  <bgfx_shader.sh>
#include "shaderlib.sh"

uniform vec4 u_scale;
/*
    Constants
*/
// log scale - so the larger = fainter stars more visible
const float MAG_LIMIT =  24.0;
const float POINT_SCALE = 0.5;
const float POINT_FALLOFF = 1.25;   //  the higher, the steeper
const float ONE_DIV_LOG10 = 1.0 / log2(10.0);
const float BRIGHTNESS_POW = 1.25;

float log10(float v) {
    return log2(v) * ONE_DIV_LOG10;
}

void main()
{
    /*  a_position is in world space.  no need to convert from model to world
    */
    vec4 cvert = mul(u_view, vec4(a_position, 1.0));
    float dist = length(cvert);
    if (dist < 0.1)
        dist = 0.1;

    /*  adjust color and size based on our apparent (relative to eye) mag */
    float appMag = a_texcoord0.x - 5.0 * (1.0 - log10(dist));
    float appMagExp = 1.0 - appMag;
    float scale = POINT_SCALE * pow(POINT_FALLOFF, appMagExp);
   
    vec2 scalexy = mul(u_scale.xy, scale);
    
    /*  vertex 0:TL, 1:TR, 2:BR, 3:BL
        goal is to generate vert and texcoord0 from vertex ID and cvert
    
        we're using conditionals, but it may make sense to lookup X,Y diffs
        from a uniform table for optimization
    */

    int vidx = gl_VertexID & 3;
    if (vidx == 0)
    {
        cvert.x -= scalexy.x;
        cvert.y += scalexy.y;
        v_texcoord0.x = 0.0;
        v_texcoord0.y = 0.0;
    }
    else if (vidx == 1)
    {
        cvert.x -= scalexy.x;
        cvert.y -= scalexy.y;
        v_texcoord0.x = 1.0;
        v_texcoord0.y = 0.0;
    }
    else if (vidx == 2)
    {
        cvert.x += scalexy.x;
        cvert.y -= scalexy.y;
        v_texcoord0.x = 1.0;
        v_texcoord0.y = 1.0;
    }
    else
    {
        cvert.x += scalexy.x;
        cvert.y += scalexy.y;
        v_texcoord0.x = 0.0;
        v_texcoord0.y = 1.0;
    }
    
    float brightness = clamp((MAG_LIMIT + appMagExp)/MAG_LIMIT, 0.0, 1.0);
    v_color0 = mul(a_color0, pow(brightness, BRIGHTNESS_POW));
    
    gl_Position = mul(u_proj, cvert);
}