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

//defaults
uniform vec2 size;
uniform int pass;

uniform sampler2DRect normals;
uniform sampler2DRect debug_input;
uniform vec4 plane;

void main( void ) 
{
  vec2 normals_size = vec2(textureSize2DRect(normals,0));

  vec2 p2 = gl_TexCoord[0].xy / size * normals_size;

  vec3 normal = texture2DRect( normals, p2 ).xyz;

  vec3 plane_normal = plane.xyz; //a,b,c
  float ang = PI - angle( plane_normal, normal, false );

  gl_FragColor = vec4(vec3(ang),1.);
}

void __debug__( void ) 
{
    vec2 p2 = gl_TexCoord[0].xy;
    float _in = texture2DRect(debug_input, p2).r;
    float _out = _in / PI;
    gl_FragColor = vec4( _out, _out, _out, 1. );
}


