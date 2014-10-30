#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/Components.h"
#include "bloqs/Bloq.h"
#include "ofxLiquidFun.h"

using namespace artemis;

class ParticleSystem : public ECSsystem 
{ 

  public:

    ParticleSystem() 
    {
      addComponentType<ParticleSystemComponent>();
    };

    const float FPS = 30.0f;

    virtual void initialize() 
    {
      ps_m.init( *world );

      box2d.init();
      box2d.setGravity(0,0);
      box2d.setFPS( FPS );

      particles.setup(box2d.getWorld());
    }; 

    virtual void added(Entity &e) 
    {};

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      box2d.update();
      for (int i=0;i<bag.getCount();i++)
        processEntity( *bag.get(i) );
      particles.draw();
    };

    // entity: escena
    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("ParticleSystem") << "process entity " << e.getId();
    }; 

    ofxBox2dParticleSystem* of_particles()
    {
      return &particles;
    };

    b2ParticleSystem* b2_particles()
    {
      return particles.particleSystem;
    };

    void physics2screen( const b2Vec2& src, ofVec2f& dst ) 
    {
      dst.x = src.x * OFX_BOX2D_SCALE;
      dst.y = src.y * OFX_BOX2D_SCALE;
    };

    void screen2physics( const ofVec2f& src, b2Vec2& dst ) 
    {
      dst.x = src.x / OFX_BOX2D_SCALE;
      dst.y = src.y / OFX_BOX2D_SCALE;
    }; 

    // Rotate a vector
    // see b2Math.h
    void b2Mul(const b2Rot& q, b2Vec2& v)
    {
      v.Set( q.c * v.x - q.s * v.y, q.s * v.x + q.c * v.y );
    }

    // TODO move steer to Boids component/system?

    void steer( const b2Vec2& target, const b2Vec2& loc, const b2Vec2& vel, float maxspeed, float maxforce, b2Vec2& force ) 
    {

      b2Vec2 desired = target - loc; // A vector pointing from the location to the target

      // Distance from the target is the magnitude of the vector
      float d = desired.Normalize(); //Returns the length

      // If the distance is greater than 0, calc steering (otherwise return zero vector)
      if (d <= 0.0f) 
      {
        force.SetZero();
        return;
      }

      //desired /= d; // Normalize desired
      desired *= maxspeed;

      // Steering = Desired minus Velocity
      force = desired - vel;

      // Limit to maximum steering force
      if ( maxforce > 0 )
      {
        float len = force.Normalize();
        force *= len > maxforce ? maxforce : len;
      }
    };

    b2Vec2 steer( const b2Vec2& target, const b2Vec2& loc, const b2Vec2& vel, float maxspeed, float maxforce ) 
    {
      b2Vec2 force;
      steer( target, loc, vel, maxspeed, maxforce, force );
      return force;
    };

  private:

    ofxBox2d box2d; 
    ofxBox2dParticleSystem particles;

    ComponentMapper<ParticleSystemComponent> ps_m;

};

