#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "components/Components.h"
#include "systems/ParticleSystem.h"

using namespace artemis;

class ParticleFlowFieldSystem : public ECSsystem 
{ 

  public:

    ParticleFlowFieldSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<ParticleFlowFieldComponent>();
      addComponentType<ParticleSystemComponent>();
    };

    virtual void initialize() 
    {
      particle_flowfield_m.init( *world );
    };

    // entity: escena
    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("ParticleFlowFieldSystem") << "update";

      FlowFieldComponent* flowfield_data = component<FlowFieldComponent>("input");
      ofVec2f *field = flowfield_data->field;
      //float param = particle_flowfield_m.get(e)->param;

      b2ParticleSystem* b2ps = system<ParticleSystem>()->b2_particles();

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

    virtual void render_entity(Entity &e)
    {};

  private:

    ComponentMapper<ParticleFlowFieldComponent> particle_flowfield_m;

};

