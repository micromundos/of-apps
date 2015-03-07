#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "utils/Config.h"

class ConfigComponent : public ECScomponent
{
  public:

    Config config;

    ConfigComponent(string _id) : ECScomponent(_id)
    {};
};

