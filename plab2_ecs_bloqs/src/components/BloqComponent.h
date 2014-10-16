#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "bloqs/Bloq.h"

class BloqComponent : public artemis::Component 
{
  public:
    string name;
    Bloq* bloq;

    BloqComponent(string name)
    {
      this->name = name;
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

