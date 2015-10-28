#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ofxSyphon.h"

class SyphonComponent : public ECScomponent 
{
  public:

    ofParameter<bool> send;
    ofParameter<bool> receive;
    ofParameter<float> fps;

    string receiver_server, receiver_app;
    string sender_server_surfaces;
    string sender_server_depth_map;
    unsigned long long prev;

    ofxSyphonServer server_surfaces;
    ofxSyphonServer server_depth_map;
    ofxSyphonClient client;

    SyphonComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      receiver_server = d["receiver"]["server"].asString();
      receiver_app = d["receiver"]["app"].asString();

      sender_server_surfaces = d["sender"]["server_surfaces"].asString();
      sender_server_depth_map = d["sender"]["server_depth_map"].asString();

      param(send, "send");
      param(receive, "receive");
      param(fps, "fps");
    };

};

