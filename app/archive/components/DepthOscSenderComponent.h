#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ofxOsc.h"

class DepthOscSenderComponent : public ECScomponent 
{
  public:

    ofxOscSender sender;
    ofParameter<bool> run;
    string host;
    int port;

    DepthOscSenderComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      host = d.get("host","localhost").asString();
      port = d.get("port",12345).asInt();
      sender.setup(host, port);
      param(run, "run");
    };

};

