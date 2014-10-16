#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class ParticleSystemComponent : public artemis::Component 
{
  public:
    string name;

    ParticleSystemComponent( string name )
    {
      this->name = name;
    };

};

