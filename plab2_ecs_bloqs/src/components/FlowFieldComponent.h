#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class FlowFieldComponent : public artemis::Component 
{
  public:
    string id;
    ofVec2f* field;

    FlowFieldComponent( string id )
    {
      this->id = id;
    };

    void update()
    {};
};

