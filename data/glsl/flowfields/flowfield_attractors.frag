#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable

/*
 * flow field of particle attractors
 *
 * in: attractor (bloq) location/s
 * out: attraction | repulsion force
 */

#pragma include "../lib/math.glsl"

//defaults
uniform vec2 size;
uniform int pass;

uniform int attractors_size;
uniform vec2 attractors_locs[10];
uniform float attractors_force_mult[10];

void main() 
{
  vec2 loc = gl_FragCoord.xy / size;
  vec2 force = vec2(0.,0.);
  for ( int i = 0; i < attractors_size; i++ )
  {
    force += (attractors_locs[i] - loc) * attractors_force_mult[i];
  }
  force /= attractors_size;

  gl_FragColor = vec4( force, 0., 1. );
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

