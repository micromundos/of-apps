#version 120
#extension GL_ARB_texture_rectangle : enable

#pragma include "lib/math.glsl"

/*
 * process debug
 * in: normalized vectors data [-1,1]
 * out: visible data [0,1]
 */

uniform sampler2DRect data;

void main( void ) 
{
    vec2 p2 = gl_TexCoord[0].xy;
    vec4 _in = texture2DRect(data, p2);

    gl_FragColor = vec4(
      lerp2d( _in.r, -1.,1., 0.,1.),
      lerp2d( _in.g, -1.,1., 0.,1.),
      lerp2d( _in.b, -1.,1., 0.,1.),
      lerp2d( _in.a, -1.,1., 0.,1.)
    );

}

