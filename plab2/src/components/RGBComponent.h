#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class RGBComponent : public ECScomponent
{
  public:

    uint8_t* color_pix;
    int width, height;
    bool dirty;
    bool ir;

    ofParameter<bool> render;

    RGBComponent(string _id) : ECScomponent(_id)
    {
      color_pix = NULL;
      dirty = false;
    };

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( render, "render" );
      ir = d.get("ir", false).asBool();
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

