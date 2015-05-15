#version 120
#extension GL_ARB_texture_rectangle : enable

#pragma include "math.glsl"

/*
 * debugging process
 * in: data [-1,1]
 * out: visible data [0,1]
 */

uniform sampler2DRect data;

void main( void ) 
{
    vec2 p2 = gl_TexCoord[0].st;
    vec4 d = texture2DRect(data, p2);

    /*vec3 vis = 0.5 + 0.5 * normalize(d.rgb);*/
    vec3 vis = vec3(
      lerp2d( d.x, -1.,1., 0.,1.),
      lerp2d( d.y, -1.,1., 0.,1.),
      lerp2d( d.z, -1.,1., 0.,1.)
    );

    gl_FragColor = vec4( vis, 1. );
}

