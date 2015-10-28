#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "bloqs/Bloq.h"

class TagsComponent : public ECScomponent 
{
  public: 

    vector< shared_ptr<Bloq> > bloqs;

    ofParameter<bool> unity_calib_save;
    ofParameter<bool> tweak_load;
    ofParameter<bool> tweak_save;
    ofParameter<bool> tweak_reset;
    ofParameter<bool> tweak_render;

    ofParameter<bool> render;
  
    ofParameter<float> interpolation_easing_loc;
    ofParameter<float> interpolation_easing_dir;
    ofParameter<float> interpolation_easing_radians;

    string calib_rgb_file;
    string calib_depth_file;
    string calib_stereo_file;
    string tweak_file;
    string unity_calib_file;

    TagsComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param( render, "render" );

      param( unity_calib_save, "unity_calib_save" );
      param( tweak_load, "tweak_load" );
      param( tweak_save, "tweak_save" );
      param( tweak_reset, "tweak_reset" );
      param( tweak_render, "tweak_render" );

      param( interpolation_easing_loc, "interpolation_easing_loc" );
      param( interpolation_easing_dir, "interpolation_easing_dir" );
      param( interpolation_easing_radians, "interpolation_easing_radians" );

      calib_rgb_file = d["calibration"]["rgb"].asString();
      calib_depth_file = d["calibration"]["depth"].asString();
      calib_stereo_file = d["calibration"]["stereo"].asString();
      tweak_file = d["calibration"]["tweak_file"].asString();
      unity_calib_file = d["calibration"]["unity_calib_file"].asString();
    };

};

