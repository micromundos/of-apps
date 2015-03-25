#version 120

#pragma include "math.glsl"

/*
 * flowfield visualization
 * in: data 
 *  flowfield
 *  proccessed by flowfield.frag
 * out: 
 *  flowfield within a visible range
 */

uniform sampler2DRect data;

void main( void ) 
{
    vec2 p2 = gl_TexCoord[0].st;
    vec4 flowfield = texture2DRect(data, p2);

    float min = -1.;
    float max = 1.;

    vec3 flowfield_vis = vec3(
      lerp2d(flowfield.x,min,max,0.,1.),
      lerp2d(flowfield.y,min,max,0.,1.),
      lerp2d(flowfield.z,min,max,0.,1.)
    );

    gl_FragColor = vec4( flowfield_vis, 1. );
}

