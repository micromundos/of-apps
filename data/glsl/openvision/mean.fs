#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect tex;
uniform int radius;
uniform float threshold;

void main()
{
	vec3 max = vec3(0.);
	vec3 min = vec3(1.);

	for(int y=-radius; y<=radius; y++) {
		for(int x=-radius; x<=radius; x++) {
			vec3 s = texture2DRect(tex, gl_TexCoord[0].xy + vec2(x, y)).rgb;
			max = max(s, max);
			min = min(s, min);
		}
	}
	vec3 color = (max + min) * 0.5;
	float alpha = texture2DRect(tex, gl_TexCoord[0].xy).a;

	gl_FragColor = vec4(color, alpha);
}
