
#define k1 0.1236
#define k2 2842.5
#define k3 1.1863
#define k4 0.0370

#pragma include "camara_lucida/glsl/math.glsl"

// mm to mt
/*uniform float near = 500. * 0.001;*/
/*uniform float far = 4000. * 0.001;*/
/*uniform float width = 640.;*/
/*uniform float height = 480.;*/
/*uniform float depth_xoff = -8.;*/
/*uniform vec4 intrinsics; //cx, cy, fx, fy;*/

float near = 500. * 0.001;
float far = 4000. * 0.001;
float width = 640.;
float height = 480.;
float depth_xoff = -8.;

vec3 unproject( vec2 p2, float z_mm ) 
{
    /*return vec3(*/
        /*(p2.x + depth_xoff - cx) * z_mm / fx,*/
        /*(p2.y - cy) * z_mm / fy,*/
        /*z_mm*/
    /*);*/
    float XtoZ = 1.11146;
    float YtoZ = 0.83359;
    return vec3(
        ( p2.x / width - 0.5 ) * z_mm * XtoZ,
        ( p2.y / height - 0.5 ) * z_mm * YtoZ,
        z_mm
    );
}

float z_norm_to_mm( float z_norm ) 
{
    return lerp2d( z_norm, 1., 0., near, far );
}

float z_raw_to_mm( float z_raw ) 
{
    return ( k1 * tan( ( z_raw / k2 ) + k3 ) - k4 ) * 1000.;
}

