#pragma once

#include <Artemis/Artemis.h>
#include "components/BloqComponent.h"
#include "components/ParticleSystemComponent.h"
#include "components/ParticleEmitterComponent.h"
#include "systems/ParticleSystem.h"

using namespace artemis;

class ParticleEmitterSystem : public EntityProcessingSystem 
{

  private:

    ComponentMapper<BloqComponent> bloq_m;
    ComponentMapper<ParticleSystemComponent> ps_m;
    ComponentMapper<ParticleEmitterComponent> emitter_m;

  public:

    ParticleEmitterSystem() 
    {
      addComponentType<BloqComponent>();
      addComponentType<ParticleSystemComponent>();
      addComponentType<ParticleEmitterComponent>();
    };

    virtual void initialize() 
    {
      bloq_m.init( *world );
      ps_m.init( *world );
      emitter_m.init( *world );
    };

    // entity = bloq
    virtual void processEntity(Entity &e) 
    {

      Bloq* bloq = bloq_m.get(e)->bloq;

      ParticleSystem* ps = ((ParticleSystem*)world->getSystemManager()->getSystem<ParticleSystem>());
      ofxBox2dParticleSystem& particles = ps->particles;

      //b2ParticleDef pd;
      //pd.flags = flags;
      //particles->CreateParticle(pd);
      ofVec2f vel = ofVec2f(0,0);
      ofVec2f loc = bloq->loc;
      particles.createParticle(loc, vel);

      ofLogNotice("ParticleEmitterSystem") << "process entity " << e.getId() << "; bloq " << bloq->id << "; create particle at loc " << loc;
    };

};

