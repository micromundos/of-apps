
vec4 debug_height( float h ) 
{
  float h2 = h == 0. ? 0. : lerp2d( h, 0., 500., 0.2, 1. );
  return vec4( h2, h2, h2, 1. );
}

