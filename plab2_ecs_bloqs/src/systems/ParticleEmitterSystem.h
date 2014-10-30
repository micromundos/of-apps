#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/Components.h"
#include "systems/ParticleSystem.h"

using namespace artemis;

class ParticleEmitterSystem : public ECSsystem 
{ 

  public:

    ParticleEmitterSystem() 
    {
      addComponentType<BloqComponent>();
      addComponentType<ParticleEmitterComponent>();
    };

    virtual void initialize() 
    {
      bloq_m.init( *world );
      emitter_m.init( *world );

      ps = system<ParticleSystem>();
      emit_remainder = 0.0f;
    };

    // entity: bloq
    virtual void processEntity(Entity &e) 
    { 
      Bloq* bloq = bloq_m.get(e)->bloq;

      // How many (fractional) particles should we have emitted this frame?
      float	dt = (1.0f / ps->FPS);
      float rate = emitter_m.get(e)->rate;
      emit_remainder += rate * dt;

      // Keep emitting particles on this frame until we only have a fractional particle left.
      while (emit_remainder > 1.0f) 
      {
        emit_remainder -= 1.0f;
        emit( bloq );
      } 
    };

  private:

    ParticleSystem* ps;

    ComponentMapper<BloqComponent> bloq_m;
    ComponentMapper<ParticleEmitterComponent> emitter_m;

    float emit_remainder;

    void emit( Bloq* bloq )
    {
      ofxBox2dParticleSystem* ofps = ps->of_particles();

      RenderComponent* render_data = component<RenderComponent>("output");
      ofVec2f screen_loc( bloq->loc.x * render_data->width, bloq->loc.y * render_data->height );

      int32 pidx = ofps->createParticle( screen_loc.x, screen_loc.y, 0, 0 );

      //TODO emitter force param
      ofVec2f force = bloq->dir * 10;//14;
      ofps->applyForce( pidx, force );
    };

};

