#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable

/*
 * flowfield
 *
 * in: list of flowfields, all same size
 * out: integration of flowfields
 */

#pragma include "../lib/math.glsl"
#pragma include "../lib/gpgpu.glsl"

uniform float weight_container;
uniform float weight_gradient;
uniform float weight_attractors;

/*https://www.opengl.org/wiki/Data_Type_%28GLSL%29#Opaque_arrays*/
uniform sampler2DRect flowfield_container;
uniform sampler2DRect flowfield_gradient;
uniform sampler2DRect flowfield_attractors;

uniform sampler2DRect debug_input;

void main( void ) 
{
  vec2 force = vec2(0.,0.);
  force += texel( flowfield_container ).xy * weight_container;
  force += texel( flowfield_gradient ).xy * weight_gradient;
  force += texel( flowfield_attractors ).xy * weight_attractors;
  gl_FragColor = vec4( force, 0., 1. );
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

