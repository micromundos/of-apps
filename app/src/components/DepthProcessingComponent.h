#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ofxGPGPU.h"

class DepthProcessingComponent : public ECScomponent 
{

  private:
    gpgpu::Process _surfaces;
    gpgpu::Process _height_map;
    gpgpu::Process _normals;

  public:

    gpgpu::Process& surfaces() 
    { return _surfaces; };

    gpgpu::Process& height_map()
    { return _height_map; };

    gpgpu::Process& normals()
    { return _normals; };

    ofParameter<bool> render_surfaces;
    ofParameter<bool> render_normals;
    ofParameter<bool> render_height_map;
    ofParameter<bool> render_smoothed;
    //ofParameter<bool> render_normals_smoothed;
    ofParameter<bool> render_plane_angles;

    ofParameter<float> open_iter;
    ofParameter<float> close_iter;

    ofParameter<float> threshold_background;
    ofParameter<float> threshold_plane;
    ofParameter<float> threshold_near;
    ofParameter<float> threshold_far;

    ofParameter<float> gaussian_sigma;
    ofParameter<float> gaussian_kernel;

    ofParameter<float> bilateral_domain;
    ofParameter<float> bilateral_range;
    ofParameter<float> bilateral_kernel;

    //ofParameter<float> normals_bilateral_domain;
    //ofParameter<float> normals_bilateral_range;
    //ofParameter<float> normals_bilateral_kernel; 

    DepthProcessingComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      //param(open_iter, "open_iter");
      //param(close_iter, "close_iter");

      //param(threshold_background, "threshold_background");
      param(threshold_plane, "threshold_plane");
      param(threshold_near, "threshold_near");
      param(threshold_far, "threshold_far"); 

      //param(gaussian_sigma, "gaussian_sigma"); 
      //param(gaussian_kernel, "gaussian_kernel"); 

      //param(bilateral_domain, "bilateral_domain"); 
      //param(bilateral_range, "bilateral_range"); 
      //param(bilateral_kernel, "bilateral_kernel");

      //param(normals_bilateral_domain, "normals_bilateral_domain"); 
      //param(normals_bilateral_range, "normals_bilateral_range"); 
      //param(normals_bilateral_kernel, "normals_bilateral_kernel"); 

      param(render_surfaces, "render_surfaces");
      param(render_normals, "render_normals");
      param(render_height_map, "render_height_map");
      //param(render_smoothed, "render_smoothed"); 
      //param(render_normals_smoothed, "render_normals_smoothed"); 
      param(render_plane_angles, "render_plane_angles"); 
    };

};

