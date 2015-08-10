#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "bloqs/Attractor.h"
#include "events/AttractorEvents.h"

using namespace artemis;

class ParticleAttractorSystem : public ECSsystem 
{ 

  public:

    ParticleAttractorSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<ParticleAttractorComponent>();
      addComponentType<BloqComponent>();
    };

    virtual void initialize() 
    {
      particle_attractor_m.init( *world );
      bloq_m.init( *world ); 
    };

    virtual void removed(Entity &e) 
    { 
      ofNotifyEvent( AttractorEvents::removed, bloq_m.get(e)->bloq->id );
    };

    virtual void added(Entity &e) 
    {
      ofNotifyEvent( AttractorEvents::added, make_attractor(e) );
    };

    virtual void processEntity(Entity &e) 
    {
      ofNotifyEvent( AttractorEvents::updated, make_attractor(e) );
    }; 

    virtual void renderEntity(Entity &e)
    {};

  private:

    ComponentMapper<ParticleAttractorComponent> particle_attractor_m;
    ComponentMapper<BloqComponent> bloq_m;  

    Attractor& make_attractor(Entity &e)
    {
      ParticleAttractorComponent* attr_data = particle_attractor_m.get(e);
      Bloq* bloq = bloq_m.get(e)->bloq;
      static Attractor attr;
      attr.id = bloq->id;
      attr.loc.set( bloq->loc );
      attr.force_mult = attr_data->force_mult;
      return attr;
    };

};

