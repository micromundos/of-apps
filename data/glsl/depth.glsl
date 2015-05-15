#pragma include "math.glsl"

//see cml::RenderShader

const float epsilon = 1e-30;

uniform float near;
uniform float far;
uniform float far_clamp;
uniform float width;
uniform float height;
uniform float fx;
uniform float fy;
uniform float cx;
uniform float cy;
uniform float xoff;

vec3 unproject( vec2 p2, float z ) 
{
  return vec3( 
    (p2.x + xoff - cx) * z / fx, 
    (p2.y - cy) * z / fy, z 
  );
}

vec3 depth_to_p3( vec2 p2, float depth_mm, bool flip )
{
  float zmm = clamp( (depth_mm < epsilon ? far_clamp : depth_mm), 0.0, far_clamp );
  vec3 p3 = unproject( p2, zmm );
  if ( !flip )
  {
    p3.x *= -1;
    p3.y *= -1;
  }
  return p3;
}

