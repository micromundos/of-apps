#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class FisicaComponent : public artemis::Component 
{
  public:
    string id;

    FisicaComponent( string id )
    {
      this->id = id;
    };
};

