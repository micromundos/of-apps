#pragma once

#include "ofxLiquidFun.h"
#include <Artemis/Artemis.h>
#include "bloqs/Bloq.h"
#include "components/ParticleSystemComponent.h"

using namespace artemis;

class ParticleSystem : public EntityProcessingSystem 
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

    virtual void begin() 
    {
      ofLogNotice("ParticleSystem") << "update";
      box2d.update();
    };

    virtual void processEntity(Entity &e) 
    {
      ofLogNotice("ParticleSystem") << "process entity " << e.getId();
    }; 

    virtual void end() 
    {
      particles.draw();
    };

};

