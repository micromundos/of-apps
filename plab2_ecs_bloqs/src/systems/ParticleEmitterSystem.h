#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/BloqComponent.h"
#include "components/ParticleEmitterComponent.h"
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
    };

    // entity: bloq
    virtual void processEntity(Entity &e) 
    {

      Bloq* bloq = bloq_m.get(e)->bloq;
      ofVec2f loc = bloq->loc;

      //b2ParticleSystem* b2ps = system<ParticleSystem>()->particles->particleSystem;

      ofxBox2dParticleSystem& ps = system<ParticleSystem>()->particles;
      float f = 14;
      float ang = bloq->angle;
      ofVec2f force( f*cos(ang), f*sin(ang) );
      int32 pidx = ps.createParticle( loc.x, loc.y, 0, 0 );
      ps.applyForce( pidx, force );

      // render
      ofSetLineWidth(2);
      ofSetColor( ofColor::green );
      ofLine( loc, loc + force.getNormalized() * 20 );

      ofLogNotice("ParticleEmitterSystem") << "process entity " << e.getId() << "; bloq " << bloq->id << "; create particle at loc " << loc << "; ang " << ang << "; force " << force;
    };

  private:

    ComponentMapper<BloqComponent> bloq_m;

};

