#version 120
#extension GL_ARB_texture_rectangle : enable

#pragma include "lib/math.glsl"
#pragma include "lib/debug_height.glsl"

/*
 * height process debug
 * in: height in mm
 * out: visible data [0,1]
 */

uniform sampler2DRect debug_input;

void main( void ) 
{
  vec2 p2 = gl_TexCoord[0].xy;
  float _in = texture2DRect(debug_input, p2).r;
  gl_FragColor = debug_height(_in);
}

