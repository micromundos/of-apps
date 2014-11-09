#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class ArucoComponent : public ECScomponent
{
  public:

    ArucoComponent(string _id) : ECScomponent(_id) {};

};

