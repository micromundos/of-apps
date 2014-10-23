#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/ParticleFlowFieldComponent.h"
#include "components/FlowFieldComponent.h"
#include "systems/ParticleSystem.h"

using namespace artemis;

class ParticleFlowFieldSystem : public ECSsystem 
{ 

  public:

    ParticleFlowFieldSystem() 
    {
      addComponentType<ParticleFlowFieldComponent>();
      addComponentType<FlowFieldComponent>();
      addComponentType<ParticleSystemComponent>();
    };

    virtual void initialize() 
    {
      particle_flowfield_m.init( *world );
      flowfield_m.init( *world );
    };

    // entity: game
    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("ParticleFlowFieldSystem") << "update";

      ofVec2f *field = flowfield_m.get(e)->field;
      //float param = particle_flowfield_m.get(e)->param;

      b2ParticleSystem* b2ps = system<ParticleSystem>()->particles.particleSystem;

      int32 n = b2ps->GetParticleCount();
      b2Vec2 *locs = b2ps->GetPositionBuffer();
      //for each 0...N 
      //particle in particle group
      //for (int i = 0; i < n; i++)
      //{
        //TODO
        //const b2Vec2& force = flowfield processing;
        //b2ps->ParticleApplyForce( i, force );
      //}

    };

  private:

    ComponentMapper<ParticleFlowFieldComponent> particle_flowfield_m;
    ComponentMapper<FlowFieldComponent> flowfield_m;

};

