#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ofxGPGPU.h"

class DepthProcessingComponent : public ECScomponent 
{
  public:

    gpgpu::Process output;

    ofParameter<bool> render;
    ofParameter<bool> render_normals;
    ofParameter<bool> render_normals_smoothed;
    ofParameter<bool> render_plane_angles;

    ofParameter<float> threshold_plane;
    ofParameter<float> threshold_near;
    ofParameter<float> threshold_far;

    ofParameter<float> normals_bilateral_domain;
    ofParameter<float> normals_bilateral_range;
    ofParameter<float> normals_bilateral_kernel; 

    DepthProcessingComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param(threshold_plane, "threshold_plane");
      param(threshold_near, "threshold_near");
      param(threshold_far, "threshold_far"); 

      param(normals_bilateral_domain, "normals_bilateral_domain"); 
      param(normals_bilateral_range, "normals_bilateral_range"); 
      param(normals_bilateral_kernel, "normals_bilateral_kernel"); 

      param(render, "render");
      param(render_normals, "render_normals");
      param(render_normals_smoothed, "render_normals_smoothed"); 
      param(render_plane_angles, "render_plane_angles"); 
    };

};

