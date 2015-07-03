#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class TagsReceiverComponent : public ECScomponent 
{
  public:

    struct Tag
    {
      string id;
      float size;
      ofVec3f translation;
      vector<ofVec2f> corners;

      string str()
      {
        stringstream ss;
        ss << "[ tag "
          << "id: " << id
          << ", size: " << size
          << ", translation: " << translation
          << ", corners: " << corners.size()
          << " ]";
        return ss.str(); 
      }
    }; 

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

