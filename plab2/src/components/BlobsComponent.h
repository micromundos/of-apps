#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class BlobsComponent : public ECScomponent 
{
  public:
    vector<ofPolyline> blobs;

    ofParameter<float> threshold_near;
    ofParameter<float> threshold_far;
    ofParameter<bool> render_depth_filtered;
    ofParameter<bool> render_contour_finder;

    BlobsComponent(string _id) : ECScomponent(_id) 
    {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param(threshold_near, "threshold_near");
      param(threshold_far, "threshold_far");
      param(render_depth_filtered, "render_depth_filtered");
      param(render_contour_finder, "render_contour_finder");
    };

};

