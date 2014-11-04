#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ecs/ECScomponent.h"

class TemplateComponent : public ECScomponent 
{
  public:

    TemplateComponent(string _id) : ECScomponent(_id) {};

    virtual void data( Json::Value d )
    {
      ECScomponent::data(d);
    };

};

