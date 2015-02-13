#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class DepthComponent : public ECScomponent
{
  public:

    uint16_t* depth_pix_mm;
    uint8_t* depth_pix_grey;
    //ofTexture depth_tex;

    int width, height;
    bool dirty;
    bool ir;

    ofParameter<bool> render;

    DepthComponent(string _id) : ECScomponent(_id)
    {
      depth_pix_mm = NULL;
      depth_pix_grey = NULL;
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

    void update( uint16_t *depth_pix_mm )
    {
      this->depth_pix_mm = depth_pix_mm;
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

