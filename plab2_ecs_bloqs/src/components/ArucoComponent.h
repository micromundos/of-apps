#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class ArucoComponent : public artemis::Component 
{
  public:
    string id;

    ArucoComponent( string id )
    {
      this->id = id;
    };
};

