#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class DepthComponent : public artemis::Component 
{
  public:
    string id; 
    uint16_t* depth_pix_mm;
    int width, height;
    bool dirty;

    DepthComponent( string id ) 
    {
      this->id = id;
      depth_pix_mm = NULL;
      dirty = false;
    };

    void init( int w, int h )
    {
      this->width = w;
      this->height = h;
    };

    void update( uint16_t *depth_pix_mm )
    {
      this->depth_pix_mm = depth_pix_mm;
    };
};

