#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

/*
 * in: depth maps for foreground, background
 * out: foreground without background
 */

#pragma include "lib/math.glsl"
#pragma include "lib/gpgpu.glsl"

//height maps: distance from table
uniform sampler2DRect foreground;
uniform sampler2DRect background;

uniform float threshold;
/*uniform float zero;*/

const float zero = FLT_MIN;

void main( void ) 
{
  float fg = texel( foreground ).x;
  float bg = texel( background ).x;
  float diff = abs(fg - bg); 
  gl_FragColor = diff < threshold ? vec4(vec3(zero),1.) : vec4(vec3(fg),1.);
}

