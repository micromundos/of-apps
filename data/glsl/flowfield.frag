#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable

#pragma include "math.glsl"

/*
 * flowfield
 *
 * in: data
 *  float depth texture
 * out: 
 *  2d vector field, rgb = xyz
 *
 */

uniform sampler2DRect data;
uniform float force_amplifier;

const int kernel = 6;

void main( void ) 
{
  vec2 loc = gl_TexCoord[0].st;
  vec2 size = vec2(textureSize2DRect(data,0));

  float ndepth;
  vec2 nloc = vec2(0.,0.);
  vec2 force = vec2(0.,0.);

  int n = 0;
  int ini = -kernel / 2;
  int end = -ini;

  for ( int i = ini; i <= end; i++ )
  for ( int j = ini; j <= end; j++ )
  {
    if ( i == 0 && j == 0 )
      continue;

    nloc = loc + vec2(i,j);
    if ( nloc.x < 0 || nloc.x >= size.x || nloc.y < 0 || nloc.y >= size.y )
      continue;

    ndepth = texture2DRect( data, nloc ).r;
    // * ndepth: drives far away from the plane
    // * -ndepth: drives towards the plane
    force += vec2(i,j) * ndepth;
    n++;
  }

  if (n > 0 && force.x != 0 && force.y != 0) 
  {
    force /= n; 
    force = normalize(force) * 1.0;
  }

  gl_FragColor = vec4( force, 0., 1. );

  /*float depth = texture2DRect(data,loc).r;*/
  /*float vis = lerp2d( depth, 0.,200., 0.,1.);*/
  /*gl_FragColor = vec4(vis,vis,vis,1.);*/
}
