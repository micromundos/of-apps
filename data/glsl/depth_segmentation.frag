#version 120

/*
 * in: height_map data
 *  in mm
 * out: 
 *  height map in mm without floor
 */

uniform sampler2DRect height_map;

uniform float threshold_near;
uniform float threshold_far;

void main( void ) 
{
  float height = texture2DRect( height_map , gl_TexCoord[0].st ).r;
  height = height < threshold_far && height > threshold_near ? height : 0.;
  gl_FragColor = vec4(vec3(height),1.);
}
