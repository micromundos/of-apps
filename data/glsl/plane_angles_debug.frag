#version 120
#extension GL_ARB_texture_rectangle : enable

#pragma include "lib/math.glsl"

/*
 * plane angles process debug
 * in: angles [0,PI]
 * out: visible data [0,1]
 */

uniform sampler2DRect data;

void main( void ) 
{
    vec2 p2 = gl_TexCoord[0].xy;
    float _in = texture2DRect(data, p2).r;
    float _out = _in / PI;
    gl_FragColor = vec4( _out, _out, _out, 1. );
}

