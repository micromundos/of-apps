#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class FisicaComponent : public ECScomponent
{
  public:
    FisicaComponent(string _id) : ECScomponent(_id) {};
};

