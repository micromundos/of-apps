#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/Components.h"
#include "systems/Systems.h"

using namespace artemis;

class ParticleEmitterSystem : public ECSsystem 
{ 

  public:

    ParticleEmitterSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<BloqComponent>();
      addComponentType<ParticleEmitterComponent>();
    };

    virtual void initialize() 
    {
      bloq_m.init( *world );
      emitter_m.init( *world );

      fisica = system<FisicaSystem>();
      ps = system<ParticleSystem>();

      emit_remainder = 0.0f;
    };

    // entity: bloq
    virtual void processEntity(Entity &e) 
    { 
      Bloq* bloq = bloq_m.get(e)->bloq;

      RenderComponent* render_data = component<RenderComponent>("output");

      // How many (fractional) particles should we have emitted this frame?
      float	dt = (1.0f / fisica->FPS);
      float rate = emitter_m.get(e)->rate;
      emit_remainder += rate * dt;

      // Keep emitting particles on this frame until we only have a fractional particle left.
      while (emit_remainder > 1.0f) 
      {
        emit_remainder -= 1.0f;
        emit( bloq, render_data );
      } 
    };

  private:

    FisicaSystem* fisica;
    ParticleSystem* ps;

    ComponentMapper<BloqComponent> bloq_m;
    ComponentMapper<ParticleEmitterComponent> emitter_m;

    float emit_remainder;

    void emit( Bloq* bloq, RenderComponent* render_data )
    {
      ofxBox2dParticleSystem* ofps = ps->of_particles();

      ofVec2f screen_loc( bloq->loc.x * render_data->width, bloq->loc.y * render_data->height );

      int32 pidx = ofps->createParticle( screen_loc.x, screen_loc.y, 0, 0 );

      //TODO emitter force param
      ofVec2f force = bloq->dir * 10;//14;
      ofps->applyForce( pidx, force );
    };

};

