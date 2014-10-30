#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class ParticleDepthBlobsContainersComponent : public artemis::Component 
{
  public:
    string id;
    float data;

    ParticleDepthBlobsContainersComponent( string id )
    {
      this->id = id;
    };

    void update( float data )
    {
      this->data = data;
    };
};

