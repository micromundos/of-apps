#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class RGBComponent : public artemis::Component 
{
  public:
    string id; 
    uint8_t* color_pix;
    int width, height;
    bool dirty;

    RGBComponent( string id ) 
    {
      this->id = id;
      color_pix = NULL;
      dirty = false;
    };

    void init( int w, int h )
    {
      this->width = w;
      this->height = h;
    };

    void update( uint8_t *color_pix )
    {
      this->color_pix = color_pix;
    };
};

