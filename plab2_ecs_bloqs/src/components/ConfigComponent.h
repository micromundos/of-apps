#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxJSON.h"

class ConfigComponent : public artemis::Component 
{
  public:
    string id;
    ofxJSONElement config;

    ConfigComponent( string id )
    {
      this->id = id;
      if ( ! config.open("config.json") )
        ofLogError("ConfigComponent") << "error opening config.json";
    };
};

