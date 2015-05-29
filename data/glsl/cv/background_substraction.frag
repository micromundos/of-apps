#version 120
#extension GL_ARB_texture_rectangle : enable

/*
 * in: foreground, background
 * out: foreground without background
 */

uniform sampler2DRect foreground;
uniform sampler2DRect background;
uniform float threshold;

void main( void ) 
{
  float F = texture2DRect( foreground, gl_TexCoord[0].xy ).x;
  float B = texture2DRect( background, gl_TexCoord[0].xy ).x;
  float diff = F-B;
  gl_FragColor = diff < threshold ? vec4(0.) : vec4(F,F,F,1.);
}

