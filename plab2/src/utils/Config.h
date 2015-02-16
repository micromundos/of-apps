#pragma once

#include "ofxJSON.h"

class Config
{
  public:

    Config()
    {
      if ( !_motor.open("../../../config/motor.json") ) ofLogError("Config") << "error opening config.json";

      if ( !_game.open("../../../config/game.json") ) ofLogError("Config") << "error opening config.json";

      if (!_settings.open("../../../settings.json")) ofLogError("Config") << "error opening settings.json";
    };

    const ofxJSONElement& motor()
    {
      return _motor;
    };

    const ofxJSONElement& game()
    {
      return _game;
    };

    const ofxJSONElement& settings()
    {
      return _settings;
    };

  private:
    ofxJSONElement _motor;
    ofxJSONElement _game;
    ofxJSONElement _settings;
};

