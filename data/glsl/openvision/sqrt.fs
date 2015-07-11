#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect tex;

void main()
{
	vec4 a = texture2DRect(tex, gl_TexCoord[0].xy);
	gl_FragColor = sqrt(a);
}
