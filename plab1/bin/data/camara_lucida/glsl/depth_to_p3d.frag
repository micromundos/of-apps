#version 120

/*
 * depth to 3d points
 *
 * in: tex0
 *  float depth texture, rgba = [0,1]
 * out: 
 *  3d points in metric space, rgb = xyz
 *
 * TODO 
 * in: tex1
 *  LUT raw2mm: z_raw_to_mm
 */

#pragma include "camara_lucida/glsl/depth_to_p3d.glsl"

uniform sampler2DRect tex0;

void main( void ) 
{
    vec2 p2 = gl_TexCoord[0].st;
    float depth = texture2DRect( tex0, p2 ).r;
    float z_mm = z_norm_to_mm( depth );
    vec3 p3 = unproject( p2, z_mm );
    gl_FragColor = vec4( p3, 1. );
}

