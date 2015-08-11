#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable

#pragma include "../lib/math.glsl"
#pragma include "../lib/gpgpu.glsl"

uniform sampler2DRect edges;
uniform sampler2DRect debug_input;

const int kernel = 6;

void main( void ) 
{
  vec2 edges_size = texsize(edges);
  vec2 loc_edges = location()*edges_size;

  float height = texel(edges,loc_edges).r;
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
    vec2 nloc_edges = loc_edges + ndir;

    if ( nloc_edges.x < 0 || nloc_edges.x >= edges_size.x || nloc_edges.y < 0 || nloc_edges.y >= edges_size.y )
      continue;

    float nheight = texel( edges, nloc_edges ).r;

    float slope = height - nheight;
    force += normalize(ndir) * slope;
    n++;
  }

  if (n > 0) 
  {
    force /= n; 
  }

  gl_FragColor = vec4( force, 0., 1.);
}

void __debug__( void ) 
{
    vec3 _in = texel(debug_input).xyz;

    vec3 _out;

    float r = 1.;
    _out = vec3(
      lerp2d( _in.x, -r,r, 0.,1.),
      lerp2d( _in.y, -r,r, 0.,1.),
      lerp2d( _in.z, -r,r, 0.,1.)
    );

    gl_FragColor = vec4( _out, 1.);
}

