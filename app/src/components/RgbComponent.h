#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class RgbComponent : public ECScomponent
{
  public:

    ofPixels* pixels;

    int width, height;
    bool dirty; 

    ofParameter<bool> render;

    RgbComponent(string _id) : ECScomponent(_id)
    {
      pixels = NULL;
      dirty = false;
    };

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( render, "render" );
      width = d.get("width",640).asInt();
      height = d.get("height",480).asInt(); 
    };

    void setup( int w, int h )
    {
      this->width = w;
      this->height = h;
    };

    void update( ofPixels& pixels )
    {
      this->pixels = &pixels;
    };

};

