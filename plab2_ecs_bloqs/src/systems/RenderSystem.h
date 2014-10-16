#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "components/MeshComponent.h"

using namespace artemis;

class RenderSystem : public EntityProcessingSystem 
{

  private:

    ComponentMapper<MeshComponent> mesh_m;

  public:

    RenderSystem() 
    {
      addComponentType<MeshComponent>();
    };

    virtual void initialize() 
    {
      mesh_m.init( *world );
    };

    virtual void processEntity(Entity &e) 
    {
      ofMesh& mesh = mesh_m.get(e)->mesh;
      mesh.draw();

      ofLogNotice("RenderSystem") << "process entity " << e.getId() << "; mesh verts size " << ofToString(mesh.getVertices().size());
    };

};

