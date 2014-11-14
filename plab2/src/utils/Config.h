#pragma once

#include "ofxJSON.h"

class Config
{
  public:

    Config()
    {
      if ( !_json.open("../../../config.json") )
        ofLogError("Config") << "error opening config.json";

      if (!_settings.open("../../../settings.json"))
        ofLogError("Config") << "error opening settings.json";
    };

    const ofxJSONElement& json()
    {
      return _json;
    };

    const ofxJSONElement& settings()
    {
      return _settings;
    };

  private:
    ofxJSONElement _json;
    ofxJSONElement _settings;
};

