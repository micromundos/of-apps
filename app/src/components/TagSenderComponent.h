#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class TagSenderComponent : public ECScomponent 
{
  public:

    string  host;
    int port;

    TagSenderComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      host = d["host"].asString();
      port = d["port"].asInt();
    };

};

