#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "bloqs/Bloq.h"

class BloqComponent : public ECScomponent
{
  private:
    Bloq _bloq;

  public:

    BloqComponent(string _id) : ECScomponent(_id) 
    {};

    //~BloqComponent()
    //{};

    Bloq* bloq()
    {
      return &_bloq;
    };

    void update( Bloq& bloq )
    {
      this->_bloq = bloq; //copy
    };
};

