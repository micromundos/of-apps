#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ofxKinect.h"

class DepthComponent : public ECScomponent
{
  public:

    ofShortPixels* depth_ofpix_mm; //uint16_t
    ofPixels* depth_ofpix_grey; //uint8_t
    ofFloatPixels* f_depth_ofpix_mm; //float
    ofFloatImage f_depth_img;

    ofxKinect kinect; 
    int width, height;
    int channels;
    bool dirty;

    ofParameter<bool> render;
    int angle; //ofParameter<float> angle;
    bool flip; //ofParameter<bool> flip;

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
      //param(angle, "angle");
      angle = -2;
      //param(flip, "flip");
      flip = false;
    };

    void setup( int w, int h )
    {
      this->width = w;
      this->height = h;
      this->channels = 1;
    }; 

};

