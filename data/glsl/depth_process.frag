
/* DEPRECATED */
/* DEPRECATED */
/* DEPRECATED */
/* DEPRECATED */
/* DEPRECATED */

#version 120
#extension GL_ARB_texture_rectangle : enable

/*
 * depth processes merged into a single shader
 *
 * 1) unproject depth to 3d points
 * 2) make a height map in mm from calibrated plane
 * 3) segment the floor plane 
 */

/*#pragma include "math.glsl"*/
#pragma include "geom.glsl"
#pragma include "depth.glsl"

uniform sampler2DRect depth_map;
uniform bool depth_flip;
uniform vec4 plane;
uniform float threshold_near;
uniform float threshold_far;

void main( void ) 
{
  vec2 p2 = gl_TexCoord[0].xy;
  float depth_mm = texture2DRect(depth_map,p2).r;

  //1) depth_3d.frag
  vec3 p3 = depth_to_p3( p2, depth_mm, depth_flip );

  //2) height_map.frag
  float height = plane_distance( plane, p3 );

  //3) depth_segmentation.frag
  height = height < threshold_far && height > threshold_near ? height : 0.;
  height = height > 0.0 ? 200.0 : 0.0;

  //write output
  gl_FragColor = vec4(vec3(height),1.);

  /*float vis = lerp2d( height, 0.,200., 0.,1.);*/
  /*gl_FragColor = vec4(vis,vis,vis,1.);*/
}

