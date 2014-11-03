#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ecs/ECScomponent.h"

class ParticleBlobsContainersComponent : public ECScomponent
{
  public:

    ParticleBlobsContainersComponent(string _id) : ECScomponent(_id) {};
};

