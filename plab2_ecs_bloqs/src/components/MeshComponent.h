#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class MeshComponent : public artemis::Component 
{
  public:
    string id;
    ofMesh mesh;

    MeshComponent( string id )
    {
      this->id = id;
      mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    };
};

