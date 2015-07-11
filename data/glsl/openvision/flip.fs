#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D tex;
uniform vec2 dim;

void main()
{
	gl_FragColor = gl_Color * texture2D(tex, dim * gl_TexCoord[0].xy);
}
