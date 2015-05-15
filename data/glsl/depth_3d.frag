#version 120
#extension GL_ARB_texture_rectangle : enable

/*
 * unproject depth to 3d points
 *
 * in: depth_map data
 *  float depth texture
 * out: 
 *  3d points in metric space
 */

#pragma include "depth.glsl"

uniform sampler2DRect depth_map;
uniform bool depth_flip;

void main( void ) 
{
  vec2 p2 = gl_TexCoord[0].st;
  float depth_mm = texture2DRect(depth_map,p2).r;

  vec3 p3 = depth_to_p3( p2, depth_mm, depth_flip );

  gl_FragColor = vec4( p3, 1. );

  /*float r = 500.;*/
  /*vec3 vis = vec3(*/
    /*lerp2d( p3.x, -r,r, 0.,1.),*/
    /*lerp2d( p3.y, -r,r, 0.,1.),*/
    /*lerp2d( p3.z, -r,r, 0.,1.)*/
  /*);*/
  /*gl_FragColor = vec4( vis, 1. );*/
}

