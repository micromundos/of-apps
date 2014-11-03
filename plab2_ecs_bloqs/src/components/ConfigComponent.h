#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ecs/ECScomponent.h"

class ConfigComponent : public ECScomponent
{
  public:
    ofxJSONElement config;

    ConfigComponent(string _id) : ECScomponent(_id)
    {
      if ( ! config.open("config.json") )
        ofLogError("ConfigComponent") << "error opening config.json";
    };
};

