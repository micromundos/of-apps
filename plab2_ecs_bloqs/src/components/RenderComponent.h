#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class RenderComponent : public artemis::Component 
{
  public:
    string id;
    int width, height;

    RenderComponent( string id )
    {
      this->id = id;
    };
};

