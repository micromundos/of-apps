#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class DepthSmoothingComponent : public ECScomponent 
{
  public:

    ofPixels output;

    ofParameter<bool> render;

    DepthSmoothingComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param(render, "render");
    };

};

