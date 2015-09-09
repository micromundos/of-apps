#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class ParticleEmitterComponent : public ECScomponent 
{
  public:

    ofParameter<float> rate;
    ofParameter<float> force;
  
    //  draw
    ofParameter<float>  draw_radius;
    ofParameter<float>  draw_weight;

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

