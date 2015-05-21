#version 120
#extension GL_ARB_texture_rectangle : enable

/*
 * in: 
 *  height_map in mm
 * out: 
 *  segmented height map in mm
 */

#pragma include "math.glsl"
#pragma include "geom.glsl"

uniform sampler2DRect height_map;
uniform sampler2DRect normals;

uniform vec4 plane;
uniform float threshold_plane;
uniform float threshold_near;
uniform float threshold_far;

void main( void ) 
{
  vec2 p2 = gl_TexCoord[0].st;

  float height = texture2DRect( height_map , p2 ).r;

  //clamp vertices too near/far from plane
  height = height < threshold_far && height > threshold_near ? height : 0.;

  //TODO testttttt
  //clamp vertices too perpendicular to plane
  /*vec3 normal = texture2DRect( normals, p2 ).xyz;*/
  /*vec3 plane_normal = plane.xyz; //a,b,c*/
  /*float ang = angle( normal, plane_normal, false );*/
  /*height = abs(degrees(ang)) < threshold_plane ? height : 0.;*/

  gl_FragColor = vec4(vec3(height),1.);
}

