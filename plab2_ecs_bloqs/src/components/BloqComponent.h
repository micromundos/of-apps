#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ecs/ECScomponent.h"
#include "bloqs/Bloq.h"

class BloqComponent : public ECScomponent
{
  public:
    Bloq* bloq;

    BloqComponent(string _id) : ECScomponent(_id) {};

    ~BloqComponent()
    {
      this->bloq = NULL;
    };

    void update( Bloq* bloq )
    {
      this->bloq = bloq;
    };
};

