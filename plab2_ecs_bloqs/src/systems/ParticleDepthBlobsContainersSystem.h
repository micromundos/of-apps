#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/Components.h"

using namespace artemis;

class ParticleDepthBlobsContainersSystem : public ECSsystem 
{ 

  public:

    ParticleDepthBlobsContainersSystem() 
    {
      addComponentType<ParticleDepthBlobsContainersComponent>();
      addComponentType<ParticleSystemComponent>();
    };

    virtual void initialize() 
    {
      particle_depth_blobs_containers_m.init( *world );
    };

    virtual void added(Entity &e) 
    {};

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("ParticleDepthBlobsContainersSystem") << "process entity " << e.getId();

      BlobsComponent* blobs_data = component<BlobsComponent>("input");
      vector<Blob>& blobs = blobs_data->blobs;
      for ( int i = 0; i < blobs.size(); i++ )
      {
        Blob& blob = blobs[i];
      }
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      for (int i=0;i<bag.getCount();i++)
        processEntity( *bag.get(i) );
    };

  private:

    ComponentMapper<ParticleDepthBlobsContainersComponent> particle_depth_blobs_containers_m;

};

