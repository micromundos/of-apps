#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/Components.h"
#include "systems/Systems.h"
#include "bloqs/Bloq.h"
#include "ofxLiquidFun.h"

using namespace artemis;

class ParticleSystem : public ECSsystem 
{ 

  public:

    ParticleSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<FisicaComponent>();
      addComponentType<ParticleSystemComponent>();
    };

    virtual void initialize() 
    {
      ps_m.init( *world ); 
      fisica = system<FisicaSystem>();

      ofColor color(127, 200, 255);
      //world, max count, lifetime, radius, render size, color
      particles.setup( fisica->b2world(), 5000, 10.0, 6.0, 4.0, color );
    }; 

    virtual void added(Entity &e) 
    {};

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      //int len = bag.getCount();
      //for ( int i = 0; i < len; i++ )
        //processEntity( *bag.get(i) );
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("ParticleSystem") << "process entity " << e.getId();
    }; 

    virtual void render()
    {
      particles.draw();
    };

    ofxBox2dParticleSystem* of_particles()
    {
      return &particles;
    };

    b2ParticleSystem* b2_particles()
    {
      return particles.particleSystem;
    }; 

  private:

    FisicaSystem* fisica;
    ofxBox2dParticleSystem particles;

    ComponentMapper<ParticleSystemComponent> ps_m;

};

