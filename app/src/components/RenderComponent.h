#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class RenderComponent : public ECScomponent 
{
  public:

    int width, height;

    RenderComponent(string _id) : ECScomponent(_id)
    {
      width = ofGetWidth();
      height = ofGetHeight();
    };

    void update( int w, int h )
    {
      width = w;
      height = h;
    };

};

