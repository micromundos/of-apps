#pragma once

#include <Artemis/Artemis.h>
#include "ofxJSON.h"

class ECScomponent : public artemis::Component
{

  public:

    ECScomponent(string _id) : id(_id)
    {};

    string id;

    virtual void data( Json::Value d )
    {
      _data = d;
    };

    virtual Json::Value data()
    {
      return _data;
    };

  protected:

    Json::Value _data;

};

