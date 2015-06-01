#version 120
#extension GL_ARB_texture_rectangle : enable

#pragma include "lib/math.glsl"

/*
 * depth process debug
 * in: depth in mm
 * out: visible data [0,1]
 */

uniform sampler2DRect debug_input;

void main( void ) 
{
    vec2 p2 = gl_TexCoord[0].xy;
    float _in = texture2DRect(debug_input, p2).r;
    float _out = lerp2d( _in, 0., 4000., 1., 0. );
    gl_FragColor = vec4( _out, _out, _out, 1. );
}

