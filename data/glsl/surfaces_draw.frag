#version 120
#pragma include "lib/math.glsl"
uniform sampler2DRect tex0;
uniform float	margin;

void main()
{
	float _lim = 300.0;
  vec4 src = texture2DRect(tex0, gl_TexCoord[0].xy);
  vec4 src_lerped = vec4(lerp2d( src.r, 0., _lim, 0.0, 1. ),lerp2d( src.g, 0., _lim, 0.0, 1. ),lerp2d( src.b, 0., _lim, 0.0, 1. ),lerp2d( src.a, 0., _lim, 0.0, 1. ));

  float	_alpha = src_lerped.r > margin ? 1.0 : 0.0;
  gl_FragColor = vec4( .6+src_lerped.r, src_lerped.r, 0.0 , _alpha );

}