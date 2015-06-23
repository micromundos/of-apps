#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class GraphComponent : public ECScomponent 
{
  public:

    ofParameter<bool> render;
    ofParameter<float> threshold_nodes;
    ofParameter<float> threshold_nodes_offset;

    GraphComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param(render, "render");
      param(threshold_nodes, "threshold_nodes");
      param(threshold_nodes, "threshold_nodes_offset");
    };

};

