#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ecs/ECScomponent.h"

class BloqMakerComponent : public ECScomponent
{
  public:

    BloqMakerComponent(string _id) : ECScomponent(_id) {};

};

