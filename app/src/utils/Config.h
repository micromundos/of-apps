#pragma once

#include "ofxJSON.h"

class Config
{
  public:

    Config(){};

    void init()
    {
      if ( !_motor.open( "config/motor.json" ) ) 
        ofLogError("Config") << "error opening motor.json \n";

      if ( !_game.open( "config/game.json" ) )
        ofLogError("Config") << "error opening game.json \n";

      if ( !_settings.open( "config/settings.json" ) )
        ofLogError("Config") << "error opening settings.json \n";
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

