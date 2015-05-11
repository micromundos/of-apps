#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ofxGeom.h"

class PlaneCalibComponent : public ECScomponent 
{
  public:

    //output
    ofxPlane plane; 
    ofxTriangle triangle; 

    ofParameter<bool> calibrate;
    ofParameter<float> planes_num;
    ofParameter<float> radius_step;
    ofParameter<float> angle_step;
    ofParameter<bool> render_plane;
    ofParameter<bool> render_planes_list;

    PlaneCalibComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param(calibrate, "calibrate");
      param(planes_num, "planes_num");
      param(radius_step, "radius_step");
      param(angle_step, "angle_step");
      param(render_plane, "render_plane");
      param(render_planes_list, "render_planes_list");
    };

};

