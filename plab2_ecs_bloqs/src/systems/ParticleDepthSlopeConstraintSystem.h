#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/Components.h"
#include "utils/CoordMap.h"

using namespace artemis;

class ParticleDepthSlopeConstraintSystem : public ECSsystem 
{ 

  //TODO usar box2d particle userData
  //para dividir los procesos en diferentes sistemas que guardan su resultado por cada particula userData
  //-> ParticleVisionSystem
  //-> ParticleDepthSlopeConstraintRenderSystem
  //-> ParticleBoidSystem

  public:

    ParticleDepthSlopeConstraintSystem() 
    {
      addComponentType<ParticleDepthSlopeConstraintComponent>();
      addComponentType<DepthComponent>();
      addComponentType<RenderComponent>();
    };

    virtual void initialize() 
    {
      particle_depth_slope_constraint_m.init( *world );
      depth_m.init( *world );
      render_m.init( *world );

      ps = system<ParticleSystem>();
    };

    virtual void added(Entity &e) 
    {
      DepthComponent* depth = depth_m.get(e);
      RenderComponent* render_data = render_m.get(e);

      screen2depth.init( render_data->width, render_data->height, depth->width, depth->height );
    };

    // entity: escena
    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("ParticleDepthSlopeConstraintSystem") << "process entity " << e.getId();

      //particle_depth_slope_constraint_m.get(e)->data;

      DepthComponent* depth = depth_m.get(e);
      if ( ! depth->dirty ) return;

      b2ParticleSystem* b2ps = ps->b2_particles(); 

      //debug render
      ofVboMesh mesh;
      mesh.setMode(OF_PRIMITIVE_LINES);

      uint16_t *depth_pix_mm = depth->depth_pix_mm; 

      //particle loc
      ofVec2f depth_loc, screen_loc;

      //particle vision
      //[head, left, right]
      b2Vec2 *pvision = new b2Vec2[3];
      ofVec2f *pv_screen = new ofVec2f[3];
      ofVec2f *pv_depth = new ofVec2f[3];
      uint16_t *pv_dmm = new uint16_t[3];
      float *pv_slope = new float[3];

      //TODO vision calc params
      float vang = PI/4.0f;
      float vmult = 0.1;//look ahead len
      b2Rot rot1(vang);
      b2Rot rot2(-vang);

      // particles iteration
      int32 n = b2ps->GetParticleCount();
      b2Vec2 *locs = b2ps->GetPositionBuffer();
      b2Vec2 *vels = b2ps->GetVelocityBuffer();
      for (int i = 0; i < n; i++)
      {
        b2Vec2& loc = locs[i];
        b2Vec2& vel = vels[i];

        //calc particle depth
        ps->physics2screen( loc, screen_loc );
        screen2depth.dst( screen_loc, depth_loc );

        uint16_t particle_depth_mm = depth_pix_mm[ screen2depth.dst_idx( depth_loc ) ];

        //calc particle vision
        //TODO move to particle_vision component
        // vision in physics coords 
        pvision[0].Set( vel.x, vel.y );
        pvision[0] *= vmult;
        float x0 = pvision[0].x;
        float y0 = pvision[0].y;
        pvision[1].Set(x0, y0);
        ps->b2Mul( rot1, pvision[1] );
        pvision[2].Set(x0, y0);
        ps->b2Mul( rot2, pvision[2] );

        for ( int v = 0; v < 3; v++ )
        {
          pvision[v] += loc;
          // vision physics to screen
          ps->physics2screen( pvision[v], pv_screen[v] );

          // vision screen to depth
          screen2depth.dst( pv_screen[v], pv_depth[v] );

          // depth mm at vision
          pv_dmm[v] = depth_pix_mm[ screen2depth.dst_idx( pv_depth[v] ) ];

          // vision slopes
          // depth mm difference really
          // positive diff = lower height wrt particle
          pv_slope[v] = particle_depth_mm - pv_dmm[v];
        }

        // calc particle force
        // vision points avg weighted by slope = lean towards lower or higher heights (see slope calc)
        //TODO params
        float pv_slope_thres_mm = 10; //the particle follows lower or higher heights but if slope is too steep (regardless its sign) it will be repeled

        //====================
        //XXX DEBUGUEAR TARGET
        //====================

        b2Vec2 target; 
        target.SetZero();
        for ( int v = 0; v < 3; v++ )
        {
          //neg weight = follow higher heights (see slope calc)
          //TODO lerp2d slope to get weight?
          float weight = -pv_slope[v];
          //if slope if steeper than thres then invert the weight to flee from there
          if ( abs(pv_slope[v]) > pv_slope_thres_mm ) weight *= -1;
          target += pvision[v] * weight;
        }
        //divide by 3 to get avg
        target *= 1.0f/3.0f;

        b2Vec2 force;
        float maxspeed = 20; 
        float maxforce = 10;
        // TODO move steer to Boids component/system
        ps->steer( target, loc, vel, maxspeed, maxforce, force );
        b2ps->ParticleApplyForce( i, force );

        //debug render vision
        for ( int v = 0; v < 1; v++ )
        {
          mesh.addVertex( screen_loc );
          mesh.addColor(ofFloatColor::red);
          mesh.addVertex( pv_screen[v] );
          mesh.addColor(ofFloatColor::red);
        }

        //debug render force
        ofVec2f target_screen;
        ps->physics2screen( target, target_screen );
        mesh.addVertex( screen_loc );
        mesh.addColor(ofFloatColor::green);
        mesh.addVertex( target_screen );
        mesh.addColor(ofFloatColor::green);

      }
      // end of particles iteration

      //debug render
      ofSetLineWidth( 1 );
      mesh.draw();

      delete[] pvision;
      delete[] pv_screen;
      delete[] pv_depth;
      delete[] pv_dmm;
      delete[] pv_slope;
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      for (int i=0;i<bag.getCount();i++)
        processEntity( *bag.get(i) );
    };

  private:

    ParticleSystem* ps;
    CoordMap screen2depth;

    ComponentMapper<ParticleDepthSlopeConstraintComponent> particle_depth_slope_constraint_m;
    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<RenderComponent> render_m;

};

