#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class DepthComponent : public ECScomponent
{
  public:

    //TODO move all pixels to ofXXXPixels, see KinectSystem
    uint16_t* depth_pix_mm;
    uint8_t* depth_pix_grey;
    float* f_depth_pix_mm;
    ofPixels* depth_ofpix_grey;
    ofShortPixels* depth_ofpix_mm;
    ofFloatPixels* f_depth_ofpix_mm;
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

    void update( uint16_t *depth_pix_mm )
    {
      this->depth_pix_mm = depth_pix_mm;
    };

    void update( float *f_depth_pix_mm )
    {
      this->f_depth_pix_mm = f_depth_pix_mm;
    };

    void update( uint8_t *depth_pix_grey )
    {
      this->depth_pix_grey = depth_pix_grey;
    };

    //void update( ofTexture& depth_tex )
    //{
      ////copy texture
      //this->depth_tex = depth_tex;
    //};
};

