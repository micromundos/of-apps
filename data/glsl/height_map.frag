#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

/*
 * in: mesh3d data
 *  3d points in metric space
 * out: 
 *  height map in mm
 */

#pragma include "lib/math.glsl"
#pragma include "lib/geom.glsl"

uniform vec2 size;

uniform sampler2DRect mesh3d;
uniform sampler2DRect debug_input;
uniform vec4 plane;

void main( void ) 
{
  vec2 mesh3d_size = vec2(textureSize2DRect(mesh3d,0));

  vec2 p2 = gl_TexCoord[0].xy / size * mesh3d_size;
  vec3 p3 = texture2DRect( mesh3d, p2 ).xyz;

  float height = plane_distance( plane, p3 );
  gl_FragColor = vec4( height, height, height, 1. );

  /*float vis = lerp2d( height, 0.,100., 0.,1.);*/
  /*gl_FragColor = vec4(vis,vis,vis,1.);*/
}

void __debug__( void ) 
{
  vec2 p2 = gl_TexCoord[0].xy;
  float _in = texture2DRect(debug_input, p2).r;
  /*float _out = lerp2d( _in, 0., 500., 0., 1. );*/
  float _out = _in == 0. ? 0. : lerp2d( _in, 0., 500., 0.2, 1. );
  gl_FragColor = vec4( _out, _out, _out, 1. );
}


