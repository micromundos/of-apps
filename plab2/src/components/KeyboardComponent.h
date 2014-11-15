#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class KeyboardComponent : public ECScomponent 
{
  public:

    KeyboardComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      //data=d.get("data",1.0).asFloat();
      //set(data, "data");
    };

};

