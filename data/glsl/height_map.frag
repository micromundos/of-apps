#version 120

/*
 * in: depth_3d data
 *  3d points in metric space
 * out: 
 *  height map in mm
 */

#pragma include "geom.glsl"
#pragma include "math.glsl"

uniform sampler2DRect depth_3d;
uniform vec4 plane;

void main( void ) 
{
  vec3 p3 = texture2DRect( depth_3d, gl_TexCoord[0].st ).rgb;
  float height = plane_distance( plane, p3 );
  gl_FragColor = vec4( height, height, height, 1. );

  /*float vis = lerp2d( height, 0.,100., 0.,1.);*/
  /*gl_FragColor = vec4( vis, vis, vis, 1. );*/
}
