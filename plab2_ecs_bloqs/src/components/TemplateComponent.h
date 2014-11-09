#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class TemplateComponent : public ECScomponent 
{
  public:

    TemplateComponent(string _id) : ECScomponent(_id) {};

    virtual void init( Json::Value d, ofParameterGroup* p )
    {
      ECScomponent::init(d,p);
      //data=d.get("data",1.0).asFloat();
      //set(data, "data");
    };

};

