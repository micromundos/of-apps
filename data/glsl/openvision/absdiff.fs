#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect tex0;
uniform sampler2DRect tex1;

void main()
{
	vec4 a = gl_Color * texture2DRect(tex0, gl_TexCoord[0].xy);
	vec4 b = gl_Color * texture2DRect(tex1, gl_TexCoord[0].xy);
	vec3 c = abs(a.rgb - b.rgb);
	gl_FragColor = vec4(c, a.a);
}
