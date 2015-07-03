#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "bloqs/Bloq.h"

class TagsProcessingComponent : public ECScomponent 
{
  public:

    vector< shared_ptr<Bloq> > bloqs;

    ofParameter<bool> render;
    string calib_rgb_file;
    string calib_depth_file;
    string calib_stereo_file;

    TagsProcessingComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param( render, "render" );

      calib_rgb_file = d["calibration"]["rgb"].asString();
      calib_depth_file = d["calibration"]["depth"].asString();
      calib_stereo_file = d["calibration"]["stereo"].asString();
    };

};

