#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
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

