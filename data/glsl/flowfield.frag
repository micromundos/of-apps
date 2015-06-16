#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable

#pragma include "lib/math.glsl"

/*
 * flowfield
 *
 * in: data
 *  height map in mm
 * out: 
 *  2d vector field, rgb = xyz
 */

uniform sampler2DRect entradas;
uniform sampler2DRect data;
uniform sampler2DRect debug_input;

const int kernel = 6;

void main( void ) 
{
  vec2 loc = gl_TexCoord[0].xy;
  vec2 size = vec2(textureSize2DRect(data,0));
  float val_entrada = (1.0-texture2DRect(entradas,loc).r);
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
    force += normalize(ndir) * -slope;
    n++;
  }

  if (n > 0) 
  {
    force /= n; 
  }

  if (height < EPSILON) //on the floor
  {
    force *= val_entrada;
  }

  gl_FragColor = vec4( force, 0.,1.);

  /*float height = texture2DRect(data,loc).r;*/
  /*float vis = lerp2d( height, 0.,200., 0.,1.);*/
  /*gl_FragColor = vec4(vis,vis,vis,1.);*/
}

void __debug__( void ) 
{
    vec2 p2 = gl_TexCoord[0].xy;
    vec3 _in = texture2DRect(debug_input, p2).xyz;

    /*_in = normalize(_in);*/

    vec3 _out;

    /*if ( _in.x == 0. && _in.y == 0. )*/
    /*{*/
      /*_out = vec3(0.5,0.5,0.5);//gris*/
    /*}*/
    /*else*/
    /*{*/
      float r = 1.;
      _out = vec3(
        lerp2d( _in.x, -r,r, 0.,1.),
        lerp2d( _in.y, -r,r, 0.,1.),
        lerp2d( _in.z, -r,r, 0.,1.)
        /*0.0 //sin azul*/
      );
    /*}*/

    gl_FragColor = vec4( _out, 1.);
}

