#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class BloqMakerComponent : public artemis::Component 
{
  public:
    string id;
    float data;

    BloqMakerComponent( string id )
    {
      this->id = id;
    };

    void update( float data )
    {
      this->data = data;
    };
};

