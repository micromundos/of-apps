#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class TemplateComponent : public artemis::Component 
{
  public:
    string id;

    TemplateComponent( string id )
    {
      this->id = id;
    };
};

