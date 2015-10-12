#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "bloqs/Attractor.h"

class ParticleAttractorComponent : public ECScomponent 
{
  public:

    ofParameter<float> force;
    ofParameter<float> radius;
    ofParameter<float> draw_weight;

    Attractor attr;

    PDraw area_circle;
    PDraw direction_circle;
    ofColor draw_color;
    ofColor draw_color_2;
    int   q_counter;
    float area_scale_vel;
    float  draw_scale;
    float  draw_vel_scale;
    int total_lines;
    float circle_res;

    ParticleAttractorComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( force, "force" );
      param( radius, "radius" );
      param( draw_weight, "draw_weight");
    };

};

