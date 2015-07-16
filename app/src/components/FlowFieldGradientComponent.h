#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class FlowFieldGradientComponent : public ECScomponent 
{
  private:
    gpgpu::Process _flowfield;

  public:

    gpgpu::Process& flowfield()
    { return _flowfield; };

    ofParameter<bool> render;
    ofParameter<float> force_weight_min;
    ofParameter<float> force_weight_max;
    ofParameter<float> force_weight_max_m;

    FlowFieldGradientComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( render, "render" );
      param( force_weight_min, "force_weight_min" );
      param( force_weight_max, "force_weight_max" );
      param( force_weight_max_m, "force_weight_max_m" );
    };

};

