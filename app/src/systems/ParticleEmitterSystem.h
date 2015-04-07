#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "Systems.h"

using namespace artemis;

class ParticleEmitterSystem : public ECSsystem 
{ 

  public:

    ParticleEmitterSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<ParticleEmitterComponent>();
      addComponentType<BloqComponent>();
    };

    virtual void initialize() 
    {
      emitter_m.init( *world );
      bloq_m.init( *world );

      fisica = require_system<FisicaSystem>();
      ps = require_system<ParticleSystem>();

      initial_fps_fisica = fisica->fps();
      emit_remainder = 0.0f;
    };

    // entity: bloq
    virtual void processEntity(Entity &e) 
    { 
      //ofLogNotice("ParticleEmitterSystem") << "process entity " << e.getId();

      Bloq* bloq = bloq_m.get(e)->bloq;

      ParticleEmitterComponent* emitter_data = emitter_m.get(e);
      RenderComponent* render_data = component<RenderComponent>("output");

      // How many (fractional) particles should we have emitted this frame?
      float	dt = (1.0f / initial_fps_fisica);
      //float	dt = (1.0f / fisica->fps());
      //float	dt = (1.0f / ofGetFrameRate());
      float rate = emitter_data->rate;
      emit_remainder += rate * dt;

      // Keep emitting particles on this frame until we only have a fractional particle left.
      while (emit_remainder > 1.0f) 
      {
        emit_remainder -= 1.0f;
        emit( bloq, emitter_data, render_data );
      } 
    };

    virtual void renderEntity(Entity &e)
    {};

  private:

    FisicaSystem* fisica;
    ParticleSystem* ps;

    ComponentMapper<ParticleEmitterComponent> emitter_m;
    ComponentMapper<BloqComponent> bloq_m;

    float initial_fps_fisica;
    float emit_remainder;

    void emit( Bloq* bloq, ParticleEmitterComponent* emitter_data, RenderComponent* render_data )
    {
      //ofxBox2dParticleSystem* ofps = ps->of_particles();
      b2ParticleSystem* b2ps = ps->b2_particles();

      ofVec2f screen_loc( bloq->loc.x * render_data->width, bloq->loc.y * render_data->height );

      //int32 pidx = ofps->createParticle( screen_loc.x, screen_loc.y, 0, 0 );
      int32 pidx = ps->make_particle( screen_loc.x, screen_loc.y, 0, 0 );

      float force_m = emitter_data->force;

      //ofVec2f force = bloq->dir * force_m;
      //ofps->applyForce( pidx, force );
      b2Vec2 force( bloq->dir.x, bloq->dir.y );
      force *= force_m;
      b2ps->ParticleApplyForce( pidx, force );
    };

};

