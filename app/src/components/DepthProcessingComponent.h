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
    ofParameter<bool> render_smooth;

    ofParameter<float> threshold_plane;
    ofParameter<float> threshold_near;
    ofParameter<float> threshold_far;

    DepthProcessingComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param(threshold_plane, "threshold_plane");
      param(threshold_near, "threshold_near");
      param(threshold_far, "threshold_far");
      param(render, "render");
      param(render_normals, "render_normals");
      param(render_smooth, "render_smooth");
    };

};

