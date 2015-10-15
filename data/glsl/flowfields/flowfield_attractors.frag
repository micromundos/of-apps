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
#pragma include "../lib/gpgpu.glsl"

#define ATTRS_LEN 10

uniform int attractors_size;
uniform vec2 attractors_locs[ATTRS_LEN];
uniform float attractors_force[ATTRS_LEN];
uniform float attractors_radius[ATTRS_LEN];

uniform sampler2DRect debug_input;

void main() 
{

  if ( attractors_size == 0 )
  {
    gl_FragColor = vec4(0.,0.,0.,1.);
    return;
  }

  vec2 loc = gl_FragCoord.xy / size;
  vec2 force = vec2(0.,0.);

  for ( int i = 0; i < attractors_size; i++ )
  {
    vec2 dir = attractors_locs[i] - loc;
    float d = length(dir);
    float f = attractors_force[i];
    float r = attractors_radius[i];
    float min = sign(f) > 0. ? EPSILON : FLT_MIN;
    float max = sign(f) > 0. ? FLT_MAX : EPSILON;
    force += (dir/d) * clamp( lerp2d( d, 0., r, f, 0. ), min, max );
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

