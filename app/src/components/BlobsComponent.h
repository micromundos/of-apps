#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ofxCv.h"

class BlobsComponent : public ECScomponent 
{
  public:

    vector<ofPolyline> blobs;

    ofParameter<bool> render;

    BlobsComponent(string _id) : ECScomponent(_id) 
    {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param(render, "render");
    };

};

