#version 120
#extension GL_ARB_texture_rectangle : enable

#pragma include "math.glsl"

/*
 * depth process debug
 * in: height in mm
 * out: visible data [0,1]
 */

uniform sampler2DRect data;

void main( void ) 
{
    vec2 p2 = gl_TexCoord[0].st;
    float _in = texture2DRect(data, p2).r;
    float _out = _in == 0. ? _in : lerp2d(_in,0.,500.,0.3,1.);
    gl_FragColor = vec4(_out,_out,_out,1.);
}

