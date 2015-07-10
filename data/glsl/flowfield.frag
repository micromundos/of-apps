#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable

/*
 * flowfield
 *
 * in: data
 *  height map in mm
 * out: 
 *  2d vector field, rgb = xyz
 */

#pragma include "lib/math.glsl"

//defaults
uniform vec2 size;
uniform int pass;

uniform sampler2DRect heightmap;
/*uniform sampler2DRect entradas;*/
uniform sampler2DRect debug_input;

uniform float floor_height;
uniform float force_weight_min;
uniform float force_weight_max;

const int kernel = 6;

bool on_floor( float height )
{
  return height <= floor_height + EPSILON;
}

void main( void ) 
{
  vec2 heightmap_size = vec2(textureSize2DRect(heightmap,0));
  /*vec2 entradas_size = vec2(textureSize2DRect(entradas,0));*/

  vec2 loc = gl_TexCoord[0].xy / size; //normalized [0,1]
  vec2 loc_heightmap = loc * heightmap_size;
  /*vec2 loc_entradas = loc * entradas_size;*/

  vec2 force = vec2(0.,0.);

  float height = texture2DRect(heightmap,loc_heightmap).r;
  /*float entrada = texture2DRect(entradas,loc_entradas).r;*/

  float sign = -1.0;

  int n = 0;
  int ini = -kernel / 2;
  int end = -ini;

  for ( int i = ini; i <= end; i++ )
  for ( int j = ini; j <= end; j++ )
  {
    if ( i == 0 && j == 0 )
      continue; 

    vec2 ndir = vec2(i,j);
    vec2 nloc_heightmap = loc_heightmap + ndir;
    /*vec2 nloc_entradas = loc_entradas + ndir;*/

    /*float nentrada = texture2DRect( entradas, nloc_entradas ).r;*/

    /*if ( on_floor(height) && !on_floor(nentrada) || !on_floor(entrada) )*/
    /*{*/
      /*sign = 0.0;*/
      /*continue;*/
    /*}*/

    if ( nloc_heightmap.x < 0 || nloc_heightmap.x >= heightmap_size.x || nloc_heightmap.y < 0 || nloc_heightmap.y >= heightmap_size.y )
      continue;

    float nheight = texture2DRect( heightmap, nloc_heightmap ).r;

    // drive away from plane or towards plane
    float slope = nheight - height;
    force += normalize(ndir) * slope;
    n++;
  }

  if (n > 0) 
  {
    force /= n; 
  }

  if ( on_floor(height) )
  {
    force *= sign;
  }

  /*float weight = lerp2d( height, 0., 1000., 0., 10. );*/
  float weight = lerp2d( length(force), 0., 1., force_weight_min, force_weight_max );
  /*float weight = 1.0;*/
  force *= weight;

  gl_FragColor = vec4( force, 0.,1.);

  /*float height = texture2DRect(heightmap,loc_heightmap).r;*/
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

