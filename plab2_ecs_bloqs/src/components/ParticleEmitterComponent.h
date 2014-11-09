#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class ParticleEmitterComponent : public ECScomponent 
{
  public:

    ofParameter<float> rate;
    ofParameter<float> force;

    ParticleEmitterComponent(string _id) : ECScomponent(_id) {};

    virtual void init( Json::Value d, ofParameterGroup* p )
    {
      ECScomponent::init(d,p);
      param( rate, "rate" );
      param( force, "force" );
    };

};

