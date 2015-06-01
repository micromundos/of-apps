#version 120
#extension GL_ARB_texture_rectangle : enable

#pragma include "lib/math.glsl"
#pragma include "lib/debug_normalized.glsl"

/*
 * process debug
 * in: normalized vectors debug_input [-1,1]
 * out: visible debug_input [0,1]
 */

uniform sampler2DRect debug_input;

void main( void ) 
{
    vec2 p2 = gl_TexCoord[0].xy;
    vec4 _in = texture2DRect(debug_input, p2);
    gl_FragColor = debug_normalized(_in);
}

