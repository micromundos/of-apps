#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class FlowFieldComponent : public ECScomponent
{
  public:

    float* field;
    //ofTexture* field;
    int width, height;
    ofParameter<bool> render;

    FlowFieldComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( render, "render" );
      width = d.get("width",640).asInt();
      height = d.get("height",480).asInt();
      field = NULL;
    };
};

