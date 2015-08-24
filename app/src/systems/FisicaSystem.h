#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include <Box2D/Box2D.h>

using namespace artemis;

class FisicaSystem : public ECSsystem 
{ 

  public:

    FisicaSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<FisicaComponent>();
    };

    virtual void initialize() 
    {
      fisica_m.init( *world );
      _world = NULL; 
    };

    virtual void added(Entity &e) 
    {
      if ( _world != NULL )
      {
        ofLogWarning("FisicaSystem")
          << "entity already added";
        return;
      }

      b2Vec2 gravity;
      gravity.Set(0.0f,0.0f);

      _world = new b2World(gravity); 

      _world->SetAllowSleeping(true);
      //debugging
      _world->SetWarmStarting(false);
      _world->SetContinuousPhysics(false);
      _world->SetSubStepping(false);

      _fps = 30.0f;
      _scale = 30.0f; //see OFX_BOX2D_SCALE

      vel_iterations = 8;
      pos_iterations = 3;
      particle_iterations = 1;
      //particle_iterations = b2CalculateParticleIterations( 10, 0.04f, 1.0f / _fps );
    };

    virtual void removed(Entity &e) 
    {
      delete _world;
      _world = NULL;
    };

    virtual void processEntity(Entity &e) 
    {
      TS_START("FisicaSystem"); 

      //particle_iterations = ceil( ofGetFrameRate() / 30.0f ); 

      float	time_step = _fps > 0.0f ? (1.0f / _fps) : 0.0f;
      _world->Step( time_step,
          vel_iterations,
          pos_iterations,
          particle_iterations);

      TS_STOP("FisicaSystem");
    }; 

    virtual void renderEntity(Entity &e)
    {};

    b2World* b2world()
    {
      return _world;
    }; 

    void fps( float val )
    { _fps = val; };

    float fps() 
    { return _fps; };

    void scale( float val )
    { _scale = val; };

    float scale() 
    { return _scale; };

    //utils

    void world2screen( const b2Vec2& src, ofVec2f& dst ) 
    {
      world2screen( src.x, src.y, dst.x, dst.y );
    };

    void world2screen( float src_x, float src_y, float& dst_x, float& dst_y ) 
    {
      dst_x = src_x * _scale;
      dst_y = src_y * _scale;
    };

    void screen2world( const ofVec2f& src, b2Vec2& dst ) 
    {
      screen2world( src.x, src.y, dst.x, dst.y );
    }; 

    void screen2world( float src_x, float src_y, float& dst_x, float& dst_y ) 
    {
      dst_x = src_x / _scale;
      dst_y = src_y / _scale;
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

    b2World* _world;

    int vel_iterations;
    int pos_iterations;
    int particle_iterations;
    float _fps;
    float _scale;

    ComponentMapper<FisicaComponent> fisica_m;

};

