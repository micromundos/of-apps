#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class ArucoComponent : public ECScomponent
{
  public:

    ofParameter<bool> render;
    ofParameter<bool> render_img;
    float marker_size;
    string calib_rgb_file;
    string calib_depth_file;
    string calib_stereo_file;

    ArucoComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param( render, "render" );
      param( render_img, "render_img" );

      marker_size = d["calibration"]["marker_size"].asFloat();
      calib_rgb_file = d["calibration"]["rgb"].asString();
      calib_depth_file = d["calibration"]["depth"].asString();
      calib_stereo_file = d["calibration"]["stereo"].asString();
    };
};

