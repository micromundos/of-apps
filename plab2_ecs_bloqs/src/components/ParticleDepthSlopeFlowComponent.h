#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ecs/ECScomponent.h"

class ParticleDepthSlopeFlowComponent : public ECScomponent
{
  public:

    ParticleDepthSlopeFlowComponent(string _id) : ECScomponent(_id) {};

};

