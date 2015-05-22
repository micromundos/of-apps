#version 120
#extension GL_ARB_texture_rectangle : enable

#pragma include "math.glsl"

/*
 * flow field process debug
 * in: data
 * out: visible data [0,1]
 */

uniform sampler2DRect data;

void main( void ) 
{
    vec2 p2 = gl_TexCoord[0].st;
    vec3 _in = texture2DRect(data, p2).xyz;

    /*_in = normalize(_in);*/

    vec3 _out;

    /*if ( _in.x == 0. && _in.y == 0. )*/
    /*{*/
      /*_out = vec3(0.5,0.5,0.5);//gris*/
    /*}*/
    /*else*/
    /*{*/
      float r = 1.;
      _out = vec3(
        lerp2d( _in.x, -r,r, 0.,1.),
        lerp2d( _in.y, -r,r, 0.,1.),
        lerp2d( _in.z, -r,r, 0.,1.)
        /*0.0 //sin azul*/
      );
    /*}*/

    gl_FragColor = vec4( _out, 1.);
}

