#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class ParticleBlobsContainersComponent : public artemis::Component 
{
  public:
    string id;

    ParticleBlobsContainersComponent( string id )
    {
      this->id = id;
    };
};

