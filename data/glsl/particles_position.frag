#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable

#pragma include "math.glsl"

/*
 * flowfield
 *
 * in: data
 *  float depth texture
 * out: 
 *  2d vector field, rgb = xyz
 *
 */
uniform sampler2DRect flowfield;
uniform sampler2DRect positions;

void main( void ) 
{
  vec2 loc = gl_TexCoord[0].st;
  vec4 position = texture2DRect( positions, loc );
  vec2 size = vec2(textureSize2DRect(positions,0));
 	
  position.rg*=vec2(320.0,240.0);

  vec4 force = texture2DRect( flowfield, position.xy );

   vec3 _out = vec3(
      lerp2d( force.x, -1.,1., 0.,1.),
      lerp2d( force.y, -1.,1., 0.,1.),
      0.0
    );



  gl_FragColor = texture2DRect(flowfield,loc);
}
	