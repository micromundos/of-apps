#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "bloqs/Bloq.h"
#include "components/ParticleSystemComponent.h"
#include "ofxLiquidFun.h"

using namespace artemis;

class ParticleSystem : public ECSsystem 
{

  private:

    ofxBox2d box2d; 

    ComponentMapper<ParticleSystemComponent> ps_m;

  public:

    ofxBox2dParticleSystem particles;

    ParticleSystem() 
    {
      addComponentType<ParticleSystemComponent>();
    };

    virtual void initialize() 
    {
      ps_m.init( *world );

      box2d.init();
      box2d.setGravity(0,0);
      box2d.setFPS( 30 );

      particles.setup( box2d.getWorld() );
    }; 

    // entity: game
    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("ParticleSystem") << "process entity " << e.getId();
      box2d.update();
      particles.draw();
    }; 

};

