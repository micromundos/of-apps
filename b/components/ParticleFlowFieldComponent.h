#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class ParticleFlowFieldComponent : public ECScomponent
{
  public:

    // TODO parametrizar el comportamiento de las particulas sobre el flowfield
    float param;

    ParticleFlowFieldComponent(string _id) : ECScomponent(_id) {};

    void update( float param )
    {
      this->param = param;
    };
};

