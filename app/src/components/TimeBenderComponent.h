#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class TimeBenderComponent : public ECScomponent 
{
  public:

    ofParameter<float> coef;

    TimeBenderComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param(coef, "coef");
    };

};

