#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ecs/ECScomponent.h"

class RGBComponent : public ECScomponent
{
  public:
    uint8_t* color_pix;
    int width, height;
    bool dirty;

    RGBComponent(string _id) : ECScomponent(_id)
    {
      color_pix = NULL;
      dirty = false;
    };

    void setup( int w, int h )
    {
      this->width = w;
      this->height = h;
    };

    void update( uint8_t *color_pix )
    {
      this->color_pix = color_pix;
    };
};

