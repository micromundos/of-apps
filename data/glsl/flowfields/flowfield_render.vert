#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable

#pragma include "../lib/math.glsl"
#pragma include "../lib/gpgpu.glsl"

uniform sampler2DRect flowfield;
uniform vec2 render_size;
uniform float force_min;
uniform float force_max;

void main( void ) 
{
	gl_TexCoord[0] = gl_MultiTexCoord0;

  vec4 vert = vec4( gl_Vertex );
  vert.xy = vert.xy / size * render_size;

  if ( gl_VertexID % 2 != 0 )
  {
    vec2 force = texel(flowfield).xy;
    vert.x += lerp2d( force.x, 0., 1., force_min, force_max ); 
    vert.y += lerp2d( force.y, 0., 1., force_min, force_max ); 
  }

  gl_Position = gl_ModelViewProjectionMatrix * vert;
	gl_FrontColor = gl_Color;
}
