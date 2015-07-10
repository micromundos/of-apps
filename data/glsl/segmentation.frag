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

//defaults
uniform vec2 size;
uniform int pass;

uniform sampler2DRect height_map;
/*uniform sampler2DRect table_angles;*/
uniform sampler2DRect debug_input;

/*uniform float threshold_table_angle;*/
uniform float threshold_table_near;
uniform float threshold_table_far;

/*const float zero = 0.;*/

void main( void ) 
{
  vec2 height_map_size = vec2(textureSize2DRect(height_map,0));
  vec2 p2_height_map = gl_TexCoord[0].xy / size * height_map_size;

  /*vec2 table_angles_size = vec2(textureSize2DRect(table_angles,0));*/
  /*vec2 p2_table_angles = gl_TexCoord[0].xy / size * table_angles_size;*/

  float zero = threshold_table_near;

  float height = texture2DRect( height_map , p2_height_map ).r;

  //clamp vertices too near/far from table
  height = height < threshold_table_far && height > threshold_table_near ? height : zero;

  //clamp vertices too perpendicular to table
  /*float ang = texture2DRect( table_angles, p2_table_angles ).x;*/
  /*height = degrees(ang) < threshold_table_angle ? height : zero;*/

  gl_FragColor = vec4(vec3(height),1.);
}

void __debug__() 
{
  vec2 p2 = gl_TexCoord[0].xy;
  float _in = texture2DRect(debug_input, p2).r;
  float _out = lerp2d( _in, 0., 500., 0.2, 1. );
  /*float _out = _in == 0. ? 0. : lerp2d( _in, 0., 500., 0.2, 1. );*/
  gl_FragColor = vec4( _out, _out, _out, 1. );
}

