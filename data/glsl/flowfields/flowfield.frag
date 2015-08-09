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

//defaults
uniform vec2 size;
uniform int pass;

uniform float weight_0;
uniform float weight_1;

/*https://www.opengl.org/wiki/Data_Type_%28GLSL%29#Opaque_arrays*/
uniform sampler2DRect flowfield_0;
uniform sampler2DRect flowfield_1;

uniform sampler2DRect debug_input;

void main( void ) 
{
  vec2 ff_size = vec2(textureSize2DRect(flowfield_0,0));
  vec2 loc = gl_TexCoord[0].xy / size * ff_size;

  vec2 force = vec2(0.,0.);
  force += texture2DRect( flowfield_0, loc ).xy * weight_0;
  force += texture2DRect( flowfield_1, loc ).xy * weight_1;
  /*force /= 2;*/

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
