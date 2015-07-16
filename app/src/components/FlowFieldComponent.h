#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ofxGPGPU.h"

class FlowFieldComponent : public ECScomponent
{
  private:
    gpgpu::Process _flowfield;

  public:

    gpgpu::Process& flowfield()
    { return _flowfield; };

    float scale;
    ofParameter<bool> render;
    ofParameter<float> weight_0;
    ofParameter<float> weight_1;

    FlowFieldComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      scale = d["scale"].asFloat();
      param( render, "render" );
      param( weight_0, "weight_0" );
      param( weight_1, "weight_1" );
    };
};

