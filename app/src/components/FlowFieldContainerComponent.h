#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class FlowFieldContainerComponent : public ECScomponent 
{
  private:
    gpgpu::Process _flowfield;

  public:

    gpgpu::Process& flowfield()
    { return _flowfield; };

    ofParameter<bool> render;

    ofParameter<bool> render_edges;
    ofParameter<float> edge_detection_threshold;

    ofParameter<float> gaussian_sigma;
    ofParameter<float> gaussian_kernel;

    ofParameter<float> bilateral_domain;
    ofParameter<float> bilateral_range;
    ofParameter<float> bilateral_kernel;

    FlowFieldContainerComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param( render, "render" );
      param( render_edges, "render_edges" );
      param( edge_detection_threshold, "edge_detection_threshold" );

      param( gaussian_sigma, "gaussian_sigma" ); 
      param( gaussian_kernel, "gaussian_kernel" );

      param( bilateral_domain, "bilateral_domain" ); 
      param( bilateral_range, "bilateral_range" ); 
      param( bilateral_kernel, "bilateral_kernel" );
    };

};

