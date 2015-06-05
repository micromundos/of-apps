/*
 * process debug
 * in: normalized vectors data [-1,1]
 * out: visible data [0,1]
 */

vec4 debug_normalized( vec4 data ) 
{
    /*return 0.5 + 0.5 * normalize(data.rgba);*/
    return vec4(
      lerp2d( data.r, -1.,1., 0.,1.),
      lerp2d( data.g, -1.,1., 0.,1.),
      lerp2d( data.b, -1.,1., 0.,1.),
      lerp2d( data.a, -1.,1., 0.,1.)
    );
}

