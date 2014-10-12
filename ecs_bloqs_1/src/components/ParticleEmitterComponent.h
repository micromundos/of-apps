#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class ParticleEmitterComponent : public artemis::Component 
{
  public:
    string name;
    ofVec2f loc;

    ParticleEmitterComponent(string name)
    {
      this->name = name;
    };
};

