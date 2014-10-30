#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class ParticleDepthSlopeFlowComponent : public artemis::Component 
{
  public:
    string id;

    ParticleDepthSlopeFlowComponent( string id )
    {
      this->id = id;
    };

};

