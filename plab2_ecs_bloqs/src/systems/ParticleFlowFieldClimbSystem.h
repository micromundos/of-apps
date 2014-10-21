#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/FlowFieldComponent.h"
#include "components/ParticleFlowFieldClimbComponent.h"
#include "systems/ParticleSystem.h"
#include "systems/FlowFieldSystem.h"

using namespace artemis;

class ParticleFlowFieldClimbSystem : public ECSsystem 
{ 

  public:

    ParticleFlowFieldClimbSystem() 
    {
      addComponentType<ParticleFlowFieldClimbComponent>();
    };

    virtual void initialize() 
    {
      flowfield_m.init( *world );
    };

    // entity: particle group o part type
    virtual void processEntity(Entity &e) 
    {
      ofLogNotice("ParticleFlowFieldClimbSystem") << "update";

      //flowfield_m.get(e)->data;

      b2ParticleSystem* b2ps = system<ParticleSystem>()->particles->particleSystem;
      //b2ParticleGroup* b2ps = xxx;

      int32 n = b2ps->GetParticleCount();
      b2Vec2 *locs = b2ps->GetPositionBuffer();
      //for each 0...N 
      //particle in particle group
      //for (int i = 0; i < n; i++)
      //{
        //const b2Vec2& force = xxx;
        //b2ps->ParticleApplyForce( i, force );
      //}

    };

  private:

    ComponentMapper<FlowFieldComponent> flowfield_m;

};

