#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class DepthComponent : public ECScomponent
{
  public:

    //ofXXXPixels ptrs
    ofShortPixels* depth_ofpix_mm;
    ofFloatPixels* f_depth_ofpix_mm;
    ofPixels* depth_ofpix_grey;

    //TODO deprecate
    //data buffers ptrs
    uint16_t* depth_pix_mm;
    float* f_depth_pix_mm;
    uint8_t* depth_pix_grey;

    //textures copies
    //ofTexture depth_tex;

    int width, height;
    bool dirty;
    bool ir;
    string calibration;

    ofParameter<bool> render;

    DepthComponent(string _id) : ECScomponent(_id)
    {
      depth_pix_mm = NULL; 
      depth_pix_grey = NULL;
      f_depth_pix_mm = NULL;
      dirty = false;
    };

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( render, "render" );
      ir = d.get("ir", false).asBool();
      calibration = d.get("calibration","").asString();
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

