#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class SyphonComponent : public ECScomponent 
{
  public:
 
    ofParameter<bool> render;
    string src, dst;

    SyphonComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      src = d["src"].asString();
      dst = d["dst"].asString();
      param(render, "render");
    };

};
