#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect data;

void main()
{
  vec3 a = texture2DRect(data, gl_TexCoord[0].xy + vec2(0, -1)).rgb;
  vec3 b = texture2DRect(data, gl_TexCoord[0].xy + vec2(-1, 0)).rgb;
  vec3 c = texture2DRect(data, gl_TexCoord[0].xy + vec2(0, 1)).rgb;
  vec3 d = texture2DRect(data, gl_TexCoord[0].xy + vec2(0, 1)).rgb;

  vec3 color = max(max(a, b), max(c, d));

  float alpha = texture2DRect(data, gl_TexCoord[0].xy).a;

  gl_FragColor = vec4(color,alpha);
}

