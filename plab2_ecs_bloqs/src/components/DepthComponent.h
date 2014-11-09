#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class DepthComponent : public ECScomponent
{
  public:
    uint16_t* depth_pix_mm;
    uint8_t* depth_pix_grey;
    int width, height;
    bool dirty;

    DepthComponent(string _id) : ECScomponent(_id)
    {
      depth_pix_mm = NULL;
      depth_pix_grey = NULL;
      dirty = false;
    };

    void setup( int w, int h )
    {
      this->width = w;
      this->height = h;
    };

    void update( uint16_t *depth_pix_mm )
    {
      this->depth_pix_mm = depth_pix_mm;
    };

    void update( uint8_t *depth_pix_grey )
    {
      this->depth_pix_grey = depth_pix_grey;
    };
};

