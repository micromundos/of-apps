#version 120
#extension GL_EXT_geometry_shader4 : enable
#extension GL_EXT_gpu_shader4 : enable

#pragma include "../lib/math.glsl"

uniform sampler2DRect data;
uniform float multx;
uniform float multy;
uniform float line_long;

float map(float value,float inputMin,float inputMax,float outputMin,float outputMax)
{
  float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
  if(outputMax < outputMin){
    if( outVal < outputMax )outVal = outputMax;
    else if( outVal > outputMin )outVal = outputMin;
  }else{
    if( outVal > outputMax )outVal = outputMax;
    else if( outVal < outputMin )outVal = outputMin;
  }
  return outVal;
}

void main(void)
{

  vec4 flowfield = texture2DRect(data, gl_PositionIn[0].xy);

  vec3 p0 = gl_PositionIn[0].xyz*vec3(multx,multy,0.0);
  float len_scale = 2.0;
  vec3 p1 = p0 + ( vec3( flowfield.r * line_long * len_scale, flowfield.g * line_long * len_scale, 0.0 ) );


  // render vector

  /*float mapr = map(flowfield.r,-1.,1.,0.,1.);*/
  /*float mapg = map(flowfield.g,-1.,1.,0.,1.);*/

  float   force = length(flowfield.rg);
  vec4    output_color = vec4(1.,0.,0.,1.);

  float   angle = atan(p0.y-p1.y,p0.x-p1.x);

  gl_Position = gl_ModelViewProjectionMatrix * vec4(p0, 1.0);
  gl_FrontColor = output_color;
  EmitVertex();

  gl_Position = gl_ModelViewProjectionMatrix * vec4(p1, 1.0);
  gl_FrontColor = output_color;
  EmitVertex();


  // render arrow

  if ( force > EPSILON )
  {
    float   angle1 = (-45.0*PI/180.0)+angle;
    float   angle2 = (45.0*PI/180.0)+angle;
    float   mult = line_long*2.0;

    vec3    arrow1 = p1+vec3(cos(angle1)*mult,sin(angle1)*mult,0.0);
    vec3    arrow2 = p1+vec3(cos(angle2)*mult,sin(angle2)*mult,0.0);

    gl_Position = gl_ModelViewProjectionMatrix * vec4(arrow1, 1.0);
    gl_FrontColor = output_color;
    EmitVertex();

    gl_Position = gl_ModelViewProjectionMatrix * vec4(p1, 1.0);
    gl_FrontColor = output_color;
    EmitVertex();


    gl_Position = gl_ModelViewProjectionMatrix * vec4(arrow2, 1.0);
    gl_FrontColor = output_color;
    EmitVertex();

    gl_Position = gl_ModelViewProjectionMatrix * vec4(p1, 1.0);
    gl_FrontColor = output_color;
    EmitVertex();

  }

}

