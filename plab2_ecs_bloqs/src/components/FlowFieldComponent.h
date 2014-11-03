#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ecs/ECScomponent.h"

class FlowFieldComponent : public ECScomponent
{
  public:
    ofVec2f* field;

    FlowFieldComponent(string _id) : ECScomponent(_id) {};

};

