#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class ParticleEmitterComponent : public artemis::Component 
{
  public:
    string name;

    ParticleEmitterComponent(string name)
    {
      this->name = name;
    };
};

