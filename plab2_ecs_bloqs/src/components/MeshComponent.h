#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>

class MeshComponent : public artemis::Component 
{
  public:
    string name;
    ofMesh mesh;

    MeshComponent( string name )
    {
      this->name = name;
      mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    };
};

