#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class TemplateComponent : public ECScomponent 
{
  public:

    //float _data;
    //ofParameter<float> _param;

    TemplateComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      //_data = d.get("_data",1.0f).asFloat();
      //param(_param, "_param");
    };

};
