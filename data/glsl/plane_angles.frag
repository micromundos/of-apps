#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

/*
 * in: 
 *  normals
 * out: 
 *  angles to plane [0,PI]
 */

#pragma include "lib/math.glsl"
#pragma include "lib/geom.glsl"
#pragma include "lib/gpgpu.glsl"

uniform sampler2DRect normals;
uniform sampler2DRect debug_input;
uniform vec4 plane;

void main( void ) 
{
  vec3 normal = texel( normals ).xyz;
  vec3 plane_normal = plane.xyz; //a,b,c
  float ang = PI - angle( plane_normal, normal, false );
  gl_FragColor = vec4(vec3(ang),1.);
}

void __debug__( void ) 
{
    float _in = texel( debug_input ).r;
    float _out = _in / PI;
    gl_FragColor = vec4( _out, _out, _out, 1. );
}


