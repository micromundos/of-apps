#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class RenderSurfacesComponent : public ECScomponent 
{
  public:

    ofParameter<bool> render;
    ofParameter<float> alpha_margin;

    RenderSurfacesComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param(render, "render");
      param(alpha_margin, "alpha_margin");
    };

};

