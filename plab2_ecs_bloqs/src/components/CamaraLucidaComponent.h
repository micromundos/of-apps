#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class CamaraLucidaComponent : public artemis::Component 
{
  public:
    string id;

    int tex_width, tex_height;

    CamaraLucidaComponent( string id )
    {
      this->id = id;
    };
};

