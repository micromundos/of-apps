#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class DepthComponent : public ECScomponent
{
  public:

    ofShortPixels* depth_ofpix_mm;//uint16_t
    ofFloatPixels* f_depth_ofpix_mm;//float
    ofPixels* depth_ofpix_grey;//uint8_t

    int width, height;
    bool dirty;

    ofParameter<bool> render;
    ofParameter<bool> flip;

    DepthComponent(string _id) : ECScomponent(_id)
    {
      depth_ofpix_mm = NULL;
      f_depth_ofpix_mm = NULL;
      depth_ofpix_grey = NULL;
      dirty = false;
    };

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param(render, "render");
      param(flip, "flip");
    };

    void setup( int w, int h )
    {
      this->width = w;
      this->height = h;
    }; 

    //void update( ofTexture& depth_tex )
    //{
      ////copy texture
      //this->depth_tex = depth_tex;
    //};
};

