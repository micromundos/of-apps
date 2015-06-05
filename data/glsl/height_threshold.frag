#version 120
#extension GL_ARB_texture_rectangle : enable

/*
 * in: 
 *  height_map in mm
 * out: 
 *  clamped height map in mm
 */

#pragma include "lib/math.glsl"

uniform sampler2DRect height_map;
uniform sampler2DRect debug_input;

uniform float threshold_near;
uniform float threshold_far;
uniform bool binary;

void main( void ) 
{
  vec2 p2 = gl_TexCoord[0].xy;
  float height = texture2DRect( height_map , p2 ).r;
  height = height < threshold_far && height > threshold_near ? (binary ? 1. : height) : 0.;
  gl_FragColor = vec4(vec3(height),1.);
}

void __debug__( void ) 
{
  vec2 p2 = gl_TexCoord[0].xy;
  float _in = texture2DRect(debug_input, p2).r;
  float _out = _in == 0. ? 0. : lerp2d( _in, 0., 500., 0.2, 1. );
  gl_FragColor = vec4( _out, _out, _out, 1. );
}

