#version 120
    
uniform sampler2DRect backbuffer;
uniform sampler2DRect tex0;

uniform float time; 
uniform vec2 mouse;
uniform vec2 resolution;

void main( void ) {
    vec2 p2 = gl_TexCoord[0].st;
    float depth = texture2DRect(tex0, p2).r;
    /* vec4 depth_out = vec4(depth);
       if (depth < 0.1) depth_out = vec4(1.,1.,0.,1.);
       if (depth > 0.9) depth_out = vec4(0.,1.,1.,1.); */ 
    gl_FragColor = vec4( depth, depth, depth, 1. );
}
