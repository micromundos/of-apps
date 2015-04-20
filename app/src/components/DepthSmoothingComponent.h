#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class DepthSmoothingComponent : public ECScomponent 
{
  public:

    ofPixels output;

    ofParameter<bool> render;
    ofParameter<int> blur_size;
    float img_scale;

    DepthSmoothingComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param(render, "render");
      param(blur_size, "blur_size");
      img_scale = d.get("img_scale",1.0f).asFloat();
    };

};

