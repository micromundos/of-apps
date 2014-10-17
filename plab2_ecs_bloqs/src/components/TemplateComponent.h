#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class TemplateComponent : public artemis::Component 
{
  public:
    string id;
    float data;

    TemplateComponent( string id )
    {
      this->id = id;
    };

    void update( float data )
    {
      this->data = data;
    };
};

