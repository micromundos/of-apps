#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ofxGPGPU.h"

class DepthProcessingComponent : public ECScomponent 
{

  private:
    gpgpu::Process _depth_3d;
    gpgpu::Process _surfaces;
    gpgpu::Process _height_map;
    //gpgpu::Process _normals;
    //gpgpu::Process _table_angles;

  public:

    gpgpu::Process& depth_3d() 
    { return _depth_3d; };

    gpgpu::Process& surfaces() 
    { return _surfaces; };

    gpgpu::Process& height_map()
    { return _height_map; };

    //gpgpu::Process& normals()
    //{ return _normals; }; 

    //gpgpu::Process& table_angles()
    //{ return _table_angles; };

    ofParameter<bool> render_surfaces;
    //ofParameter<bool> render_normals;
    ofParameter<bool> render_height_map;
    ofParameter<bool> render_smoothed;
    //ofParameter<bool> render_normals_smoothed;
    //ofParameter<bool> render_table_angles;

    ofParameter<float> open_iter;
    ofParameter<float> close_iter;

    ofParameter<bool> bg_dif;
    ofParameter<float> bg_dif_threshold;
    ofParameter<float> bg_dif_expand_kernel;

    ofParameter<float> threshold_table_near;
    ofParameter<float> threshold_table_far;
    //ofParameter<float> threshold_table_angle;

    //ofParameter<float> gaussian_sigma;
    //ofParameter<float> gaussian_kernel;

    //ofParameter<float> bilateral_domain;
    //ofParameter<float> bilateral_range;
    //ofParameter<float> bilateral_kernel;

    //ofParameter<float> normals_bilateral_domain;
    //ofParameter<float> normals_bilateral_range;
    //ofParameter<float> normals_bilateral_kernel; 

    DepthProcessingComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param(open_iter, "open_iter");
      param(close_iter, "close_iter");

      param(bg_dif, "bg_dif");
      param(bg_dif_threshold, "bg_dif_threshold");
      param(bg_dif_expand_kernel, "bg_dif_expand_kernel");


      param(threshold_table_near, "threshold_table_near");
      param(threshold_table_far, "threshold_table_far"); 
      //param(threshold_table_angle, "threshold_table_angle");

      //param(gaussian_sigma, "gaussian_sigma"); 
      //param(gaussian_kernel, "gaussian_kernel"); 

      //param(bilateral_domain, "bilateral_domain"); 
      //param(bilateral_range, "bilateral_range"); 
      //param(bilateral_kernel, "bilateral_kernel");

      //param(normals_bilateral_domain, "normals_bilateral_domain"); 
      //param(normals_bilateral_range, "normals_bilateral_range"); 
      //param(normals_bilateral_kernel, "normals_bilateral_kernel"); 

      param(render_surfaces, "render_surfaces");
      //param(render_normals, "render_normals");
      param(render_height_map, "render_height_map");
      //param(render_smoothed, "render_smoothed"); 
      //param(render_normals_smoothed, "render_normals_smoothed"); 
      //param(render_table_angles, "render_table_angles"); 
    };

};

