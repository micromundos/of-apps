#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ofxGPGPU.h"

class FlowFieldComponent : public ECScomponent
{
  public:

    gpgpu::Process process;

    int width, height;
    ofParameter<bool> render;
    ofParameter<float> force_amplifier;

    FlowFieldComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( render, "render" );
      param( force_amplifier, "force_amplifier");
      width = d.get("width",640).asInt();
      height = d.get("height",480).asInt();
    };
};

