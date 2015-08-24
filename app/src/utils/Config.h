#pragma once

#include "ofxJSON.h"

class Config
{
  public:

    Config(){};

    void init()
    {
      if ( !_settings.open( "config/settings.json" ) )
        ofLogError("Config") << "error opening settings.json \n";

      if ( !_motor.open( _settings["params"]["app"]["motor_path"].asString() ) ) 
        ofLogError("Config") << "error opening motor.json \n";

      if ( !_game.open( _settings["params"]["app"]["game_path"].asString() ) ) 
        ofLogError("Config") << "error opening game.json \n"; 
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

