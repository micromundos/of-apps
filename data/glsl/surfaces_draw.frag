#version 120

uniform sampler2DRect tex0;
uniform float	margin;

void main()
{
  vec4 src = texture2DRect(tex0, gl_TexCoord[0].xy);
  float	_alpha = src.r > margin ? src.r : 0.0;
  gl_FragColor = vec4( 0.0,src.r,.3+src.r, _alpha );
}
