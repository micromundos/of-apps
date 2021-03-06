#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class DepthSegmentationComponent : public ECScomponent 
{
  public:

    ofFloatPixels output;
    //ofTexture output; //TODO

    ofParameter<bool> render;
    ofParameter<float> threshold_near;
    ofParameter<float> threshold_far;

    DepthSegmentationComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param(threshold_near, "threshold_near");
      param(threshold_far, "threshold_far");
      param(render, "render");
    };

};

