#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "bloqs/Bloq.h"

class BloqComponent : public artemis::Component 
{
  public:
    string id;
    Bloq* bloq;

    BloqComponent(string id)
    {
      this->id = id;
    };

    ~BloqComponent()
    {
      this->bloq = NULL;
    };

    void update( Bloq* bloq )
    {
      this->bloq = bloq;
    };

};

