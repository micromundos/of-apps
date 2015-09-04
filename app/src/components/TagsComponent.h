#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "bloqs/Bloq.h"

class TagsComponent : public ECScomponent 
{
  public: 

    vector< shared_ptr<Bloq> > bloqs;

    ofParameter<bool> tweak_load;
    ofParameter<bool> tweak_save;
    ofParameter<bool> tweak_reset;
    ofParameter<bool> tweak_render;

    ofParameter<bool> render;

    string calib_rgb_file;
    string calib_depth_file;
    string calib_stereo_file;

    TagsComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param( render, "render" );

      param( tweak_load, "tweak_load" );
      param( tweak_save, "tweak_save" );
      param( tweak_reset, "tweak_reset" );
      param( tweak_render, "tweak_render" );

      calib_rgb_file = d["calibration"]["rgb"].asString();
      calib_depth_file = d["calibration"]["depth"].asString();
      calib_stereo_file = d["calibration"]["stereo"].asString();
    };

};

