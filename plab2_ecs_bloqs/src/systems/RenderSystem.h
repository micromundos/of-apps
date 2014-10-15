#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "components/MeshComponent.h"

class RenderSystem : public artemis::EntityProcessingSystem 
{

  private:

    artemis::ComponentMapper<MeshComponent> mesh_m;

  public:

    RenderSystem() 
    {
      addComponentType<MeshComponent>();
    };

    virtual void initialize() 
    {
      mesh_m.init( *world );
    };

    virtual void processEntity(artemis::Entity &e) 
    {
      ofMesh& mesh = mesh_m.get(e)->mesh;
      mesh.draw();
    };

};

