#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class ParticleDepthSlopeConstraintComponent : public artemis::Component 
{
  public:
    string id;

    ParticleDepthSlopeConstraintComponent( string id )
    {
      this->id = id;
    };

};

