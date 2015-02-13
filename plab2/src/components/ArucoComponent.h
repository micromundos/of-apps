#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "bloqs/Bloq.h"

class ArucoComponent : public ECScomponent
{
  public:

    ofParameter<bool> render;
    vector< shared_ptr<Bloq> > bloqs;

    ArucoComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( render, "render" );
    };

};

