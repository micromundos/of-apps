
/*
 * glsl utils
 */

//defaults
uniform vec2 size;
uniform int pass;

vec2 texsize( sampler2DRect tex )
{
  return vec2( textureSize2DRect( tex, 0 ) );
}

//normalized [0,1]
vec2 location()
{
  return gl_TexCoord[0].xy / size;
}

vec2 location( sampler2DRect tex )
{
  return location() * texsize(tex);
}

vec4 texel( sampler2DRect tex )
{
  return texture2DRect( tex, location(tex) );
}

vec4 texel( sampler2DRect tex, vec2 loc )
{
  return texture2DRect( tex, loc );
}

