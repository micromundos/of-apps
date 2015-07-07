#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "bloqs/Tag.h"

class TagsReceiverComponent : public ECScomponent 
{
  public: 

    vector<Tag> tags;

    string ip;
    int port;

    TagsReceiverComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      ip = d["ip"].asString();
      port = d["port"].asInt();
    };

};

