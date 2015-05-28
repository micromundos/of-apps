#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class DepthComponent : public ECScomponent
{
  public:

    ofShortPixels* depth_ofpix_mm; //uint16_t
    ofFloatPixels* f_depth_ofpix_mm; //float
    ofPixels* depth_ofpix_grey; //uint8_t

    int width, height;
    int channels;
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
      this->channels = 1;
    }; 

    void update( ofShortPixels& depth_ofpix_mm )
    {
      this->depth_ofpix_mm = &depth_ofpix_mm;
    };

    void update( ofFloatPixels& f_depth_ofpix_mm )
    {
      this->f_depth_ofpix_mm = &f_depth_ofpix_mm;
    };

    void update( ofPixels& depth_ofpix_grey )
    {
      this->depth_ofpix_grey = &depth_ofpix_grey;
    };
};

