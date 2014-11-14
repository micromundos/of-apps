#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "components/Components.h"
#include "ofxLiquidFun.h"

using namespace artemis;

class FisicaSystem : public ECSsystem 
{ 

  public:

    FisicaSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<FisicaComponent>();
    };

    const float FPS = 30.0f;

    virtual void initialize() 
    {
      fisica_m.init( *world );
      box2d.init();
      box2d.setGravity(0,0);
      box2d.setFPS( FPS );
    };

    virtual void added(Entity &e) 
    {
      //fisica_m.get(e)->data;
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("FisicaSystem") << "process entity " << e.getId();
      //fisica_m.get(e)->data;
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      box2d.update();
      //int len = bag.getCount();
      //for ( int i = 0; i < len; i++ )
        //processEntity( *bag.get(i) );
    };

    virtual void render_entity(Entity &e)
    {};

    b2World* b2world()
    {
      return box2d.getWorld();
    };

    //utils

    void world2screen( const b2Vec2& src, ofVec2f& dst ) 
    {
      world2screen( src.x, src.y, dst.x, dst.y );
    };

    void world2screen( float src_x, float src_y, float& dst_x, float& dst_y ) 
    {
      dst_x = src_x * OFX_BOX2D_SCALE;
      dst_y = src_y * OFX_BOX2D_SCALE;
    };

    void screen2world( const ofVec2f& src, b2Vec2& dst ) 
    {
      screen2world( src.x, src.y, dst.x, dst.y );
    }; 

    void screen2world( float src_x, float src_y, float& dst_x, float& dst_y ) 
    {
      dst_x = src_x / OFX_BOX2D_SCALE;
      dst_y = src_y / OFX_BOX2D_SCALE;
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

    ComponentMapper<FisicaComponent> fisica_m;

};

