#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class ParticleFlowFieldComponent : public artemis::Component 
{
  public:
    string id;

    // TODO parametrizar el comportamiento de las particulas sobre el flowfield
    float param;

    ParticleFlowFieldComponent( string id )
    {
      this->id = id;
    };

    void update( float param )
    {
      this->param = param;
    };
};

