#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class FlowFieldRenderComponent : public ECScomponent 
{
  public:

    ofParameter<bool> render;
    ofParameter<float> force_min;
    ofParameter<float> force_max;

    FlowFieldRenderComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( render, "render" );
      param( force_min, "force_min" );
      param( force_max, "force_max" );
    };

};

