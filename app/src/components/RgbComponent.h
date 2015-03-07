#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class RgbComponent : public ECScomponent
{
  public:

    uint8_t* color_pix;

    int width, height;
    bool dirty;
    string calibration;
    string calibration_stereo;

    ofParameter<bool> render;

    RgbComponent(string _id) : ECScomponent(_id)
    {
      color_pix = NULL;
      dirty = false;
    };

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( render, "render" );
      width = d.get("width",640).asInt();
      height = d.get("height",480).asInt();
      calibration = d.get("calibration","").asString();
      calibration_stereo = d.get("calibration_stereo","").asString();
    };

    void setup( int w, int h )
    {
      this->width = w;
      this->height = h;
    };

    void update( uint8_t *color_pix )
    {
      this->color_pix = color_pix;
    };
};

