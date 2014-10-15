#version 120

#pragma include "camara_lucida/glsl/math.glsl"

/*
 * normals visualization
 * in: tex0 
 *  normals, rgb = xyz
 *  proccessed by normals.frag
 * out: 
 *  normals with a visible range
 */

uniform sampler2DRect tex0;

void main( void ) 
{
    vec2 p2 = gl_TexCoord[0].st;
    vec4 normal = texture2DRect( tex0, p2 );

    vec3 normal_vis = vec3(
            lerp2d( normal.x, -1., 1., 0., 1. ),
            lerp2d( normal.y, -1., 1., 0., 1. ),
            lerp2d( normal.z, -1., 1., 0., 1. )
            );

    gl_FragColor = vec4( normal_vis, 1. );
}

