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
    ofParameter<bool> render_height_smoothed;
    ofParameter<bool> render_plane_angles;

    ofParameter<float> threshold_plane;
    ofParameter<float> threshold_near;
    ofParameter<float> threshold_far;

    // output: entradas a los bloques
    gpgpu::Process entradas; 
    ofParameter<float> threshold_entradas_near;
    ofParameter<float> threshold_entradas_far;
    ofParameter<bool> render_entradas;


    DepthProcessingComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param(threshold_plane, "threshold_plane");
      param(threshold_near, "threshold_near");
      param(threshold_far, "threshold_far"); 
      param(render, "render");
      param(render_normals, "render_normals");
      param(render_normals_smoothed, "render_normals_smoothed");
      param(render_height_smoothed, "render_height_smoothed");
      param(render_plane_angles, "render_plane_angles");

      // entradas
      param(render_entradas, "render_entradas");
      param(threshold_entradas_near, "threshold_entradas_near");
      param(threshold_entradas_far, "threshold_entradas_far");
    };

};

