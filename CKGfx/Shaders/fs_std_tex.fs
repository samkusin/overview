$input v_normal, v_texcoord0, v_position

/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include  <bgfx_shader.sh>
#include "shaderlib.sh"
#include "ckgfx.sh"

SAMPLER2D(u_texColor, 0);

/*  Direct Lighting Info

    u_specularColor
    ---------------

    u_specularity
    ---------------

    u_lightParam
    ---------------
    For the basic lighting model, light param is an amalgam of:
        x = ambient, y = diffuse, z = distance, w = cutoff
        if z == 0, then directional
        else
            if w == 0, then point (omnidirectional)
            else angle between light and extent of light on surface (spot)

    u_lightCoeff
    ---------------
    For the basic lighting model point and spot lights
        x = constant
        y = linear
        z = exp

    u_lightColor
    ---------------
    The light color RGBA (A?)

    u_lightDir
    ---------------
    Light direction (spot, point also require origin)

    u_lightOrigin
    ---------------
    For the basic lighting model, the origin used in spot or point lights
*/
uniform vec4 u_lightColor[CKGFX_SHADERS_LIGHT_COUNT];
uniform vec4 u_lightDir[CKGFX_SHADERS_LIGHT_COUNT];
uniform vec4 u_lightParam[CKGFX_SHADERS_LIGHT_COUNT];
uniform vec4 u_lightCoeff[CKGFX_SHADERS_LIGHT_COUNT];
uniform vec4 u_lightOrigin[CKGFX_SHADERS_LIGHT_COUNT];

/*  Materials */
uniform vec4 u_specularity;

#define u_ambientIntensity(_light_) u_lightParam[_light_].x
#define u_diffuseIntensity(_light_) u_lightParam[_light_].y
#define u_lightDistance(_light_) u_lightParam[_light_].z
#define u_lightCutoff(_light_) u_lightParam[_light_].w


vec4 colorOfLightFromDirection(int index, vec3 direction, vec3 normal)
{
    vec4 ambientColor = u_lightColor[index] * u_ambientIntensity(index);
    vec4 diffuseColor = u_lightColor[index] * u_diffuseIntensity(index);
    vec4 specularColor = vec4(0,0,0,0);

    float diffuseScalar = dot(normal, -direction);

    if (diffuseScalar > 0.0) {
        diffuseColor = diffuseColor * diffuseScalar;
        if (u_specularity.x > 0.0) {
          vec3 eyePos = u_invView[3].xyz;
          vec3 toEye = normalize(eyePos - v_position);
          vec3 reflectDir = normalize(reflect(direction, normal));
          float specularScalar = dot(toEye, reflectDir);
          if (specularScalar > 0.0) {
              specularScalar = pow(specularScalar, u_specularity.y) * u_specularity.x;
              specularColor = u_lightColor[index] * specularScalar;
              specularColor.a = 1.0;
          }
        }
    }
    else {
        diffuseColor = vec4(0,0,0,0);
    }

    return ambientColor + diffuseColor + specularColor;
}

void main()
{
    vec4 tex0Color = texture2D(u_texColor, v_texcoord0);
    vec3 surfaceNorm = normalize(v_normal);
    vec4 totalColor = vec4(0,0,0,0);

    for (int i = 0; i < CKGFX_SHADERS_LIGHT_COUNT; ++i) {
      if (u_ambientIntensity(i) > 0.0 || u_diffuseIntensity(i) > 0.0) {
        vec3 lightDir;
        float lightCutoff = u_lightCutoff(i);
        float lightOriginDist = u_lightDistance(i);
        float distance = 0.0;
        float spotScalar = 0.0;

        if (lightOriginDist > 0.0) {
            //  point or spot
            lightDir = v_position - u_lightOrigin[i].xyz;
            distance = length(lightDir);
            lightDir = normalize(lightDir);

            if (lightCutoff > 0.0) {
                //  spot light
                spotScalar = dot(lightDir, u_lightDir[i].xyz);
            }
        }
        else {
            // directional
            lightDir = u_lightDir[i].xyz;
        }

        vec4 color;

        if (lightCutoff == 0.0 || lightCutoff > 0.0 && spotScalar > lightCutoff) {
          // directional || point || spot light within cutoff
          color = colorOfLightFromDirection(i, lightDir, surfaceNorm);
        }
        else {
          color = vec4(0,0,0,0);
        }

        if (lightOriginDist > 0.0 && color.a > 0.0) {
          float attenuation = u_lightCoeff[i].x + u_lightCoeff[i].y * distance +
                              u_lightCoeff[i].z * distance * distance;
          color = color/attenuation;
          if (lightCutoff > 0.0) {
            // lightCutoff != 1 since 1 >= spotScalar > lightCutoff
            color = color * (1.0 - (1.0 - spotScalar)/(1.0 - lightCutoff));
          }
        }
        totalColor += color;
      }
    }

    gl_FragColor = tex0Color * totalColor;
}
