#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class ParticleSystemComponent : public ECScomponent
{
  public:

    ofParameter<float> maxspeed;

    ParticleSystemComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( maxspeed, "maxspeed" );
    };
};

