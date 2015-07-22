#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable

/*
 * flowfield
 *
 * in: data
 *  height map in mm
 * out: 
 *  2d vector field
 */

#pragma include "../lib/math.glsl"

//defaults
uniform vec2 size;
uniform int pass;

uniform sampler2DRect height_map;
uniform sampler2DRect debug_input;

uniform float force_weight_min;
uniform float force_weight_max;

const int kernel = 3;

void main( void ) 
{
  vec2 height_map_size = vec2(textureSize2DRect(height_map,0));

  vec2 loc = gl_TexCoord[0].xy / size; //normalized [0,1]
  vec2 loc_height_map = loc * height_map_size;

  vec2 force = vec2(0.,0.);

  float height = texture2DRect(height_map,loc_height_map).r;

  if ( height < 0.0 )
  {
    gl_FragColor = vec4( force, 0.,1.);
    return;
  }

  int n = 0;
  int ini = -kernel / 2;
  int end = -ini;

  for ( int i = ini; i <= end; i++ )
  for ( int j = ini; j <= end; j++ )
  {
    if ( i == 0 && j == 0 )
      continue; 

    vec2 ndir = vec2(i,j);
    vec2 nloc_height_map = loc_height_map + ndir;

    if ( nloc_height_map.x < 0 || nloc_height_map.x >= height_map_size.x || nloc_height_map.y < 0 || nloc_height_map.y >= height_map_size.y )
      continue;

    float nheight = texture2DRect( height_map, nloc_height_map ).r;

    if ( nheight < 0.0 )
      continue;

    // drive away from table or towards table
    float slope = nheight - height;
    force += normalize(ndir) * slope;
    n++;
  }

  if (n > 0) 
  {
    force /= n; 
  }

  /*float weight = lerp2d( height, 0., 1000., 0., 10. );*/
  float weight = lerp2d( length(force), 0., 1., force_weight_min, force_weight_max );
  force *= weight;

  // limit force
  float max_force = 10.0;
  if ( length(force) > max_force )
  {
    force = normalize(force) * max_force;
  }

  gl_FragColor = vec4( force, 0.,1.);
}

void __debug__( void ) 
{
    vec2 p2 = gl_TexCoord[0].xy;
    vec3 _in = texture2DRect(debug_input, p2).xyz;

    vec3 _out;

    float r = 1.;
    _out = vec3(
      lerp2d( _in.x, -r,r, 0.,1.),
      lerp2d( _in.y, -r,r, 0.,1.),
      lerp2d( _in.z, -r,r, 0.,1.)
    );

    gl_FragColor = vec4( _out, 1.);
}

