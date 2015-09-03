#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

/*
 * in: 
 *  height_map in mm
 * out: 
 *  segmented height map in mm
 */

#pragma include "lib/math.glsl"
#pragma include "lib/geom.glsl"
#pragma include "lib/gpgpu.glsl"

uniform sampler2DRect height_map;
/*uniform sampler2DRect table_angles;*/
uniform sampler2DRect debug_input;

/*uniform float threshold_table_angle;*/
uniform float threshold_table_near;
uniform float threshold_table_far;

/*const float zero = 0.;*/

void main( void ) 
{
  float zero = threshold_table_near;

  float height = texel( height_map ).r;

  //clamp vertices too near/far from table
  height = height < threshold_table_far && height > threshold_table_near ? height : zero;

  //clamp vertices too perpendicular to table
  /*float ang = texel( table_angles ).x;*/
  /*height = degrees(ang) < threshold_table_angle ? height : zero;*/

  gl_FragColor = vec4(vec3(height),1.);
}

void __debug__() 
{
  float _in = texel( debug_input ).r;
  float _out = lerp2d( _in, 0., 500., 0.0, 1. );
  /*float _out = _in <= 0. ? 0. : lerp2d( _in, 0., 500., 0.2, 1. );*/
  gl_FragColor = vec4( _out, _out, _out, 1. );
}

