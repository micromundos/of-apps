#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class ParticleEmitterComponent : public artemis::Component 
{
  public:
    string id;

    ParticleEmitterComponent(string id)
    {
      this->id = id;
    };
};

