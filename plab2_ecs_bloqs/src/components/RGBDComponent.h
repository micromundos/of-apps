

//==========
//DEPRECATED
//==========


#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class RGBDComponent : public artemis::Component 
{
  public:
    string id; 
    uint16_t* depth_pix_mm;
    uint8_t* color_pix;
    int width, height;

    RGBDComponent( string id ) 
    {
      this->id = id;
    };

    void init( int w, int h )
    {
      this->width = w;
      this->height = h;
    };

    void update_depth( uint16_t *depth_pix_mm )
    {
      this->depth_pix_mm = depth_pix_mm;
    };

    void update_color( uint8_t *color_pix )
    {
      this->color_pix = color_pix;
    };

};

