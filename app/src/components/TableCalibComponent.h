#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ofxGeom.h"

class TableCalibComponent : public ECScomponent 
{
  public:

    //output
    ofxPlane plane; 
    ofxTriangle triangle; 

    ofFloatImage background_depth_map;
    ofFloatImage background_height_map;

    string filename_plane;
    string filename_background_depth_map;
    string filename_background_height_map;

    ofParameter<bool> calibrate;
    ofParameter<bool> learn_bg;
    ofParameter<bool> save;
    ofParameter<bool> load;
    ofParameter<float> planes_num;
    ofParameter<float> radius_step;
    ofParameter<float> angle_step;
    ofParameter<bool> render_plane;
    ofParameter<bool> render_planes_list;

    TableCalibComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      filename_plane = d.get("filename_plane","table_plane.yml").asString();
      filename_background_depth_map = d.get("filename_background_depth_map","table_background_depth_map.yml").asString();
      filename_background_height_map = d.get("filename_background_height_map","table_background_height_map.yml").asString();

      param(calibrate, "calibrate");
      param(learn_bg, "learn_bg");
      param(save, "save");
      param(load, "load");
      param(planes_num, "planes_num");
      param(radius_step, "radius_step");
      param(angle_step, "angle_step");
      param(render_plane, "render_plane");
      param(render_planes_list, "render_planes_list");
      
    };

};

