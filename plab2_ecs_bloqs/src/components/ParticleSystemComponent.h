#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class ParticleSystemComponent : public artemis::Component 
{
  public:
    string id;

    ParticleSystemComponent( string id )
    {
      this->id = id;
    };

};

