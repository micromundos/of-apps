#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect data;
uniform sampler2DRect mask;

void main( void ) 
{
  vec4 _depth = texture2DRect( data, gl_TexCoord[0].xy );
  vec4 _mask = texture2DRect( mask, gl_TexCoord[0].xy );

  _depth = _mask.r < 0.5 ? vec4(0.,0.,0.,1.) : _depth;
  gl_FragColor = _depth * gl_Color;
}
