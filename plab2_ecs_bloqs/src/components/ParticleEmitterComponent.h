#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ecs/ECScomponent.h"

class ParticleEmitterComponent : public ECScomponent 
{
  public:

    // particles per second
    float rate;
    // emitter force (by bloq ang) multiplier
    float force_m;

    ParticleEmitterComponent(string _id) : ECScomponent(_id) {};

    virtual void data( Json::Value d )
    {
      ECScomponent::data(d);
      rate = d.get("rate",1.0).asFloat();
      force_m = d.get("force_m",10.0).asFloat();
    };

};

