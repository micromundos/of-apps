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
    ofParameter<float> max_force;

    ofParameter<float> gaussian_sigma;
    ofParameter<float> gaussian_kernel;

    FlowFieldGradientComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param( render, "render" );
      param( max_force, "max_force" );

      param( gaussian_sigma, "gaussian_sigma" ); 
      param( gaussian_kernel, "gaussian_kernel" );
    };

};

