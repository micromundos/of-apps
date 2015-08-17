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
    ofParameter<float> repel_amp;
    ofParameter<float> repel_min_limit;
    ofParameter<float> repel_max_limit;
    ofParameter<float> dif_max_positive;


    FlowFieldComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( render, "render" );
      width = d.get("width",640).asInt();
      height = d.get("height",480).asInt();
      param(repel_amp, "repel_amp");
      param(repel_min_limit,"repel_min_limit");
      param(repel_max_limit,"repel_max_limit");
      param(dif_max_positive,"dif_max_positive");
      field = NULL;
    };
};

