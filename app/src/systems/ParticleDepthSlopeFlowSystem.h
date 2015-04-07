#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "Systems.h"
#include "utils/CoordMap.h"

using namespace artemis;

class ParticleDepthSlopeFlowSystem : public ECSsystem 
{ 

  //TODO usar box2d particle userData
  //para dividir los procesos en diferentes sistemas que guardan su resultado por cada particula userData
  //-> ParticleVisionSystem
  //-> ParticleDepthSlopeFlowRenderSystem
  //-> ParticleBoidSystem

  public:

    ParticleDepthSlopeFlowSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<ParticleDepthSlopeFlowComponent>();
      addComponentType<ParticleSystemComponent>();
    };

    virtual void initialize() 
    {

      //TODO que es mejor, traer un componente de una entidad taggeada o un sistema? 
      //ParticleSystemComponent* ps_data = component<ParticleSystemComponent>("particles");
      //ofps = ps_data->of_particles();
      //b2ps = ps_data->b2_particles();

      fisica = require_system<FisicaSystem>();
      ps = require_system<ParticleSystem>();

      mesh.setMode(OF_PRIMITIVE_LINES);
    };

    virtual void added(Entity &e) 
    {};

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      //pudo haber cambiado el screen size
      update_screen2depth();
      mesh.clear();
      int len = bag.getCount();
      for ( int i = 0; i < len; i++ )
        processEntity( *bag.get(i) );
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("ParticleDepthSlopeFlowSystem") << "process entity " << e.getId();

      DepthComponent* depth_data = require_component<DepthComponent>("input");
      if ( ! depth_data->dirty ) return;

      b2ParticleSystem* b2ps = ps->b2_particles();  

      uint16_t *depth_pix_mm = depth_data->depth_pix_mm; 

      //particle loc
      ofVec2f depth_loc, screen_loc;

      //particle vision triangle
      //[ head, left, right ]
      b2Vec2 *pvision = new b2Vec2[3];
      ofVec2f *pv_screen = new ofVec2f[3];
      ofVec2f *pv_depth = new ofVec2f[3];
      uint16_t *pv_dmm = new uint16_t[3];
      float *pv_slope = new float[3];

      //TODO vision calc params
      float vang = PI/4.0f; //45deg
      float vlen = 0.4; //vision length
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
        fisica->world2screen( loc, screen_loc );
        screen2depth.dst( screen_loc, depth_loc );

        uint16_t particle_depth_mm = depth_pix_mm[ screen2depth.dst_idx( depth_loc ) ];

        //calc particle vision
        //TODO move to particle_vision component
        //vision in physics coords 
        //head
        pvision[0].Set( vel.x, vel.y );
        pvision[0].Normalize();
        pvision[0] *= vlen;
        float x0 = pvision[0].x;
        float y0 = pvision[0].y;
        //left
        pvision[1].Set(x0, y0);
        fisica->b2Mul(rot1, pvision[1]);
        //right
        pvision[2].Set(x0, y0);
        fisica->b2Mul(rot2, pvision[2]);

        for ( int v = 0; v < 3; v++ )
        {
          pvision[v] += loc;

          // vision physics to screen
          fisica->world2screen( pvision[v], pv_screen[v] );

          // vision screen to depth
          screen2depth.dst( pv_screen[v], pv_depth[v] );

          // depth mm at vision
          pv_dmm[v] = depth_pix_mm[ screen2depth.dst_idx( pv_depth[v] ) ];

          // vision slopes
          // depth mm difference really
          // positive slope = lower height wrt particle
          pv_slope[v] = particle_depth_mm - pv_dmm[v];
        }

        //calc particle force
        //vision points avg weighted by slope = lean towards lower or higher heights (see slope calc)
        //TODO params
        float pv_slope_thres_mm = 100; //the particle follows lower or higher heights but if slope is too steep (regardless its sign) it will be repeled

        b2Vec2 target; 
        target.SetZero();
        for ( int v = 0; v < 3; v++ )
        {
          float weight = pv_slope[v] * 0.001;
          if (abs(pv_slope[v]) > pv_slope_thres_mm) weight *= -1.0;
          target += (pvision[v] - loc) * weight;
        }
        //divide by 3 to get avg
        target *= (1.0f/3.0f);
        target += loc;

        b2Vec2 force;
        float maxspeed = 10; 
        float maxforce = 8;
        fisica->steer( target, loc, vel, maxspeed, maxforce, force );
        b2ps->ParticleApplyForce( i, force );

        ofFloatColor _color;

        //debug render vision
        //_color = ofFloatColor::red;
        //for ( int v = 0; v < 3; v++ )
        //{
          //mesh.addVertex(screen_loc);
          //mesh.addColor(_color);
          //mesh.addVertex(pv_screen[v]);
          //mesh.addColor(_color);
        //}

        //debug render force
        _color = ofFloatColor::green;
        ofVec2f target_screen;
        fisica->world2screen(target, target_screen);
        mesh.addVertex( screen_loc );
        mesh.addColor(_color);
        mesh.addVertex( target_screen );
        mesh.addColor(_color);

      }
      // end of particles iteration 

      delete[] pvision;
      delete[] pv_screen;
      delete[] pv_depth;
      delete[] pv_dmm;
      delete[] pv_slope;
    }; 

    virtual void renderEntity(Entity &e)
    {
      ofSetLineWidth(1);
      mesh.draw();
    };

  private:

    FisicaSystem* fisica;
    ParticleSystem* ps;

    CoordMap screen2depth;

    ofVboMesh mesh;

    void update_screen2depth()
    {
      DepthComponent* depth_data = require_component<DepthComponent>("input");
      RenderComponent* render_data = require_component<RenderComponent>("output");
      screen2depth.set( render_data->width, render_data->height, depth_data->width, depth_data->height );
    };

};

