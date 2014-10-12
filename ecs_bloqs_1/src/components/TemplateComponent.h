#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class TemplateComponent : public artemis::Component 
{
  public:
    string name;
    float data;

    TemplateComponent( string name )
    {
      this->name = name;
    };

    void update( float data )
    {
      this->data = data;
    };
};

