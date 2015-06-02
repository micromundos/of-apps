#version 120
#extension GL_ARB_texture_rectangle : enable

#pragma include "../lib/math.glsl"

uniform sampler2DRect data;
uniform sampler2DRect mask;

void main( void ) 
{
  vec2 p2 = gl_TexCoord[0].xy;
  vec4 d = texture2DRect( data, p2 );
  float m = texture2DRect( mask, p2 ).r;
  gl_FragColor = m < EPSILON ? vec4(0.,0.,0.,1.) : vec4(d);
}
