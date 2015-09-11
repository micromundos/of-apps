#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "PDraw.h"

class ParticleEmitterComponent : public ECScomponent 
{
  public:

    ofParameter<float> rate;
    ofParameter<float> force;
  
    //  draw
    ofParameter<float>  draw_radius;
    ofParameter<float>  draw_weight;
  
    PDraw  draw_circle;
    PDraw  draw_direction,draw_direction_3;
    ofColor circle_color;
    ofColor direction_color;
    ofColor direction_color_l;
    int    draw_resolution;
    float  draw_scale;
    float  draw_vel_scale;
    float  draw_scale_d;
    bool   draw_inited;


    ParticleEmitterComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( rate, "rate" );
      param( force, "force" );
      
      param(draw_radius , "draw_radius");
      param(draw_weight , "draw_weight");

    };

};

