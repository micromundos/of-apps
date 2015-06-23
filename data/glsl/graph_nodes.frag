#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

/*
 * in: 
 *  height_map in mm
 * out: 
 *  graph nodes
 */

#pragma include "lib/math.glsl"
#pragma include "lib/geom.glsl"

uniform vec2 size;

uniform sampler2DRect height_map;
uniform sampler2DRect plane_angles;
uniform float threshold;
uniform float offset;

void main( void ) 
{
  vec2 height_map_size = vec2(textureSize2DRect(height_map,0));
  vec2 p2_height_map = gl_TexCoord[0].xy / size * height_map_size;

  vec2 plane_angles_size = vec2(textureSize2DRect(plane_angles,0));
  vec2 p2_plane_angles = gl_TexCoord[0].xy / size * plane_angles_size;

  float height = texture2DRect( height_map , p2_height_map ).r;

  float ang = degrees( texture2DRect( plane_angles, p2_plane_angles ).x );
  height = ang > threshold-offset && ang < threshold+offset ? 1.0 : 0.0;

  gl_FragColor = vec4(vec3(height),1.);
}

