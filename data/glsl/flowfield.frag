#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable

#pragma include "math.glsl"

/*
 * flowfield
 *
 * in: data
 *  height map in mm
 * out: 
 *  2d vector field, rgb = xyz
 */

uniform sampler2DRect data;

const int kernel = 6;

void main( void ) 
{
  vec2 loc = gl_TexCoord[0].xy;
  vec2 size = vec2(textureSize2DRect(data,0));

  float height = texture2DRect(data,loc).r;

  vec2 force = vec2(0.,0.);

  int n = 0;
  int ini = -kernel / 2;
  int end = -ini;

  for ( int i = ini; i <= end; i++ )
  for ( int j = ini; j <= end; j++ )
  {
    if ( i == 0 && j == 0 )
      continue;

    vec2 ndir = vec2(i,j);
    vec2 nloc = loc + ndir;

    if ( nloc.x < 0 || nloc.x >= size.x || nloc.y < 0 || nloc.y >= size.y )
      continue;

    float nheight = texture2DRect( data, nloc ).r;
    float slope = height - nheight;

    // nheight: drives away from plane
    // -nheight: drives towards plane
    force += normalize(ndir) * slope;
    n++;
  }

  if (n > 0) 
  {
    force /= n; 
  }

  if (height < EPSILON) //on the floor
  {
    force *= -1.0;
  }

  gl_FragColor = vec4( force, 0.,1.);

  /*float height = texture2DRect(data,loc).r;*/
  /*float vis = lerp2d( height, 0.,200., 0.,1.);*/
  /*gl_FragColor = vec4(vis,vis,vis,1.);*/
}
