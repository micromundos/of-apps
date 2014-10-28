#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class ParticleEmitterComponent : public artemis::Component 
{
  public:
    string id;

    // Particles per second
    float rate;

    ParticleEmitterComponent(string id)
    {
      this->id = id;
      rate = 1.0;
    };
};

