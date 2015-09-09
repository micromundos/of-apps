#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class ParticleVolumeSensorComponent : public ECScomponent 
{
  public:

    float volume; 
    float volume_i; //lerped
    float volume_n; 
    float volume_n_i; //lerped
    float volume_prev; 

    ofParameter<bool> render;
    ofParameter<float> radius;
    ofParameter<float> vol_view_mult;

    ParticleVolumeSensorComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param(render, "render");
      param(radius, "radius");
      param(vol_view_mult, "vol_view_mult");
    };

};

