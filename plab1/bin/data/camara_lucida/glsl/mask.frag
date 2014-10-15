#version 120

uniform sampler2DRect tex0;
uniform sampler2DRect tex1;

void main( void ) 
{
    vec4 depth = texture2DRect( tex0, gl_TexCoord[0].st );
    vec4 mask = texture2DRect( tex1, gl_TexCoord[0].st );

    depth = mask.r < 0.5 ? vec4(0.,0.,0.,1.) : depth;
    gl_FragColor = depth * gl_Color;
}
