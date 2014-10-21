
// NOT USED FTTB

#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/MeshComponent.h"
#include "ofMain.h"

using namespace artemis;

class RenderSystem : public ECSsystem 
{ 

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

  private:

    ComponentMapper<MeshComponent> mesh_m;

};

