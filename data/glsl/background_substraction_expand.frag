#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

/*
 * in: height maps for foreground, background
 * out: foreground without background
 * chequea si está sobre el borde de un bloque 
 * (i.e. algún vecino está por encima del threshold) 
 * - si está sobre un borde: deja foreground
 * - si no: escribe zero
 */

#pragma include "lib/math.glsl"
#pragma include "lib/gpgpu.glsl"

uniform sampler2DRect backbuffer;

//height maps: distance from table
uniform sampler2DRect foreground;
uniform sampler2DRect background;

uniform int kernel;
/*uniform float zero;*/

const float zero = 0.0;

void main( void ) 
{
  vec2 loc_bbf = location( backbuffer );
  /*vec2 loc_bg = location(background); */

  float fg = texel( foreground ).x;
  float bbf = texel( backbuffer, loc_bbf ).x;

  vec4 _out;
  vec4 fg4 = vec4(vec3(fg),1.);
  vec4 zero4 = vec4(vec3(zero),1.);

  if ( bbf < zero + EPSILON )
  {
    bool on_border = false;
    for ( int y = -kernel; y <= kernel; y++ ) 
    {
      for ( int x = -kernel; x <= kernel; x++ ) 
      {
        if (x == 0 && y == 0) continue;

        vec2 nloc_bbf = loc_bbf + vec2( x, y );
        /*vec2 nloc_bg = loc_bg + vec2( x, y );*/

        float nbbf = texel( backbuffer, nloc_bbf ).x;
        if ( nbbf > zero )
        {
          _out = fg4;
          on_border = true;
          break;
        }
      }
      if (on_border) break;
    }

    // todos los vecinos son zero
    if ( !on_border )
      _out = zero4;
  }

  // dejar foreground
  else
  {
    _out = fg4;
  }

  gl_FragColor = _out;
}

