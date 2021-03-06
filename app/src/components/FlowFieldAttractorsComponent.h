#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class FlowFieldAttractorsComponent : public ECScomponent 
{

  private:
    gpgpu::Process _flowfield;

  public:

    gpgpu::Process& flowfield()
    { return _flowfield; };

    ofParameter<bool> render;
    ofParameter<float> scale;

    FlowFieldAttractorsComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( render, "render" );
      param( scale, "scale" );
    };

};

