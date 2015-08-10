#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class ParticleAttractorComponent : public ECScomponent 
{
  public:

    ofParameter<float> force_mult;

    ParticleAttractorComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( force_mult, "force_mult" );
    };

};

