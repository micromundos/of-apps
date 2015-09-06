#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class FlowFieldRenderComponent : public ECScomponent 
{
  public:

    ofParameter<bool> render;
    ofParameter<float> force_mult;

    FlowFieldRenderComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( render, "render" );
      param( force_mult, "force_mult" );
    };

};

