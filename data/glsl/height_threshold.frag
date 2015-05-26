#version 120
#extension GL_ARB_texture_rectangle : enable

/*
 * in: 
 *  height_map in mm
 * out: 
 *  clamped height map in mm
 */

uniform sampler2DRect height_map;

uniform float threshold_near;
uniform float threshold_far;

void main( void ) 
{
  vec2 p2 = gl_TexCoord[0].xy;

  float height = texture2DRect( height_map , p2 ).r;

  height = height < threshold_far && height > threshold_near ? height : 0.;

  gl_FragColor = vec4(vec3(height),1.);
}

