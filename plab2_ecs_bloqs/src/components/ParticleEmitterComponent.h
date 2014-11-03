#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ecs/ECScomponent.h"

class ParticleEmitterComponent : public ECScomponent 
{
  public:

    // Particles per second
    float rate;

    ParticleEmitterComponent(string _id) : ECScomponent(_id) {};

    virtual void data( Json::Value d )
    {
      ECScomponent::data(d);
      rate = d.get("rate",1.0).asFloat();
    };

};

