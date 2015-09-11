#version 120
#pragma include "lib/math.glsl"
uniform sampler2DRect tex0;
uniform float	margin;

void main()
{
	float _lim = 300.0;
 // vec4 src = texture2DRect(tex0, gl_TexCoord[0].xy);
 /*	
 float	_a = src.r > margin*_lim ? 300.0 : 0.0;
 src.a = _a;

 vec4 src_lerped = vec4(lerp2d( src.r, 0., _lim, 0.0, 1. ),lerp2d( src.g, 0., _lim, 0.0, 1. ),lerp2d( src.b, 0., _lim, 0.0, 1. ),lerp2d( src.a, 0., _lim, 0.0, 1. ));*/



 float blurSize = 2.0;
 vec4 sum = vec4(0.0);

 sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y - 4.0 * blurSize)) * 0.049049;
 sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y - 3.0 * blurSize)) * 0.0882;
 sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y - 2.0 * blurSize)) * 0.1176;
 sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y - blurSize)) * 0.147;
 sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y)) * 0.196;
 sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + blurSize)) * 0.147;
 sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + 2.0 * blurSize)) * 0.1176;
 sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + 3.0 * blurSize)) * 0.0882;
 sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + 4.0 * blurSize)) * 0.049049;
 /*
sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x - 4.0 * blurSize, gl_TexCoord[0].y)) * 0.049049;
sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x - 3.0 * blurSize, gl_TexCoord[0].y)) * 0.0882;
sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x - 2.0 * blurSize, gl_TexCoord[0].y)) * 0.1176;
sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x - blurSize, gl_TexCoord[0].y)) * 0.147;
sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y)) * 0.196;
sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x + blurSize, gl_TexCoord[0].y)) * 0.147;
sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x + 2.0 * blurSize, gl_TexCoord[0].y)) * 0.1176;
sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x + 3.0 * blurSize, gl_TexCoord[0].y)) * 0.0882;
sum += texture2DRect(tex0, vec2(gl_TexCoord[0].x + 4.0 * blurSize, gl_TexCoord[0].y)) * 0.049049;*/

 vec4 src = sum;
  vec4 src_lerped = vec4(lerp2d( src.r, 0., _lim, 0.0, 1. ),lerp2d( src.g, 0., _lim, 0.0, 1. ),lerp2d( src.b, 0., _lim, 0.0, 1. ),lerp2d( src.a, 0., _lim, 0.0, 1. ));

  float	_alpha = src_lerped.r > margin ? 1.0 : 0.0;
  //vec4 src = texture2DRect(tex0, gl_TexCoord[0].xy);


 gl_FragColor = vec4( .5+src_lerped.r, .5, 0.0 , src_lerped.r );





}
