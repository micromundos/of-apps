#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

/*
 * in: depth maps for foreground, background
 * out: foreground without background
 */

uniform vec2 size;

uniform sampler2DRect foreground;
uniform sampler2DRect background;
uniform float threshold;
/*uniform float zero;*/

const float zero = 0.0; //5000.0;

void main( void ) 
{
  vec2 foreground_size = vec2(textureSize2DRect(foreground,0));
  vec2 background_size = vec2(textureSize2DRect(background,0));

  vec2 loc_F = gl_TexCoord[0].xy / size * foreground_size;
  vec2 loc_B = gl_TexCoord[0].xy / size * background_size;

  float F = texture2DRect( foreground, loc_F ).x;
  float B = texture2DRect( background, loc_B ).x;

  /*float zero = B - threshold;*/

  float diff = B-F; //depth map: distance to camera

  gl_FragColor = diff < threshold ? vec4(vec3(zero),1.) : vec4(F,F,F,1.);
}

