#pragma once

#include <Artemis/Artemis.h>
#include "bloqs/Bloq.h"
#include "components/BloqComponent.h"
#include "components/ParticleEmitterComponent.h"

class ParticleSystem : public artemis::EntityProcessingSystem 
{

  private:

    artemis::ComponentMapper<BloqComponent> bloq_m;
    artemis::ComponentMapper<ParticleEmitterComponent> emitter_m;

  public:

    ParticleSystem() 
    {
      addComponentType<BloqComponent>();
      addComponentType<ParticleEmitterComponent>();
    };

    virtual void initialize() 
    {
      bloq_m.init( *world );
      emitter_m.init( *world );
    };

    virtual void processEntity(artemis::Entity &e) 
    {
      Bloq* bloq = bloq_m.get(e)->bloq;
      emitter_m.get(e)->loc.set( bloq->loc );
      ofLogNotice("ParticleSystem") << "process entity on bloq " << ofToString(bloq->id) << " at loc " << bloq->loc;
    };

};

