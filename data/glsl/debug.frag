#version 120
#extension GL_ARB_texture_rectangle : enable

#pragma include "math.glsl"

/*
 * process debug
 * in: data [-1,1]
 * out: visible data [0,1]
 */

uniform sampler2DRect data;

void main( void ) 
{
    vec2 p2 = gl_TexCoord[0].st;
    vec4 _in = texture2DRect(data, p2);

    /*vec3 _out = 0.5 + 0.5 * normalize(_in.rgb);*/
    vec3 _out = vec3(
      lerp2d( _in.x, -1.,1., 0.,1.),
      lerp2d( _in.y, -1.,1., 0.,1.),
      0.0
    );



    gl_FragColor = vec4( _out, 1. );
}

