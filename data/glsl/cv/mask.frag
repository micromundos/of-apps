#version 120
#extension GL_ARB_texture_rectangle : enable

#pragma include "../lib/math.glsl"
#pragma include "../lib/gpgpu.glsl"

uniform sampler2DRect data;
uniform sampler2DRect mask;

void main( void ) 
{
  vec4 d = texel( data );
  float m = texel( mask ).r;
  gl_FragColor = m < EPSILON ? vec4(0.,0.,0.,1.) : vec4(d);
}
