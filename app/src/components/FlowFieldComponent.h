#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class FlowFieldComponent : public ECScomponent
{
  public:
    ofVec2f* field;

    FlowFieldComponent(string _id) : ECScomponent(_id) {};

};

