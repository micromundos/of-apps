#version 120
#extension GL_ARB_texture_rectangle : enable

/*
 * in: 
 *  height_map in mm
 * out: 
 *  segmented height map in mm
 */

#pragma include "lib/math.glsl"
#pragma include "lib/geom.glsl"

uniform sampler2DRect height_map;
uniform sampler2DRect plane_angles;

uniform float threshold_plane;
uniform float threshold_near;
uniform float threshold_far;

void main( void ) 
{
  vec2 p2 = gl_TexCoord[0].xy;

  float height = texture2DRect( height_map , p2 ).r;

  //clamp vertices too near/far from plane
  height = height < threshold_far && height > threshold_near ? height : 0.;

  //clamp vertices too perpendicular to plane
  float ang = texture2DRect( plane_angles, p2 ).x;
  height = degrees(ang) < threshold_plane ? height : 0.;

  gl_FragColor = vec4(vec3(height),1.);
}

