#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "Systems.h"
#include "bloqs/Bloq.h"

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
      fisica = require_system<FisicaSystem>();
      init(); 
    };  

    virtual void added(Entity &e) 
    {};

    virtual void processEntity(Entity &e)
    {
      ParticleSystemComponent* ps_data = ps_m.get(e);

      //limit particles speed

      float maxspeed = ps_data->maxspeed;

      if ( maxspeed == 0.0 )
        return;

      int32 n = b2particles->GetParticleCount();
      b2Vec2 *vels = b2particles->GetVelocityBuffer(); 
      for (int i = 0; i < n; i++)
      {
        b2Vec2& vel = vels[i];
        float len = vel.Normalize();
        vel *= len > maxspeed ? maxspeed : len;
        //cout << i << ": vel len " << len << ", max " << maxspeed << endl;
      }
    }; 

    void renderEntities(ImmutableBag<Entity*>& bag) 
    {
      //EntityProcessingSystem::renderEntities(bag);

      mesh.clear();

      int32 n = b2particles->GetParticleCount();
      b2Vec2 *posbuf = b2particles->GetPositionBuffer();
      b2ParticleColor *colbuf = b2particles->GetColorBuffer();

      ofVec2f loc;
      ofFloatColor col;
      for ( int i = 0; i < n; i++ ) 
      {
        loc.set( posbuf[i].x, posbuf[i].y );
        col.set( colbuf[i].r / 255.0, colbuf[i].g / 255.0, colbuf[i].b / 255.0 );
        mesh.addVertex( loc );
        mesh.addColor( col );
      }

      ofSetColor(255);
      ofPushMatrix();
      ofScale( fisica->scale(), fisica->scale() );
      glPointSize( render_size );
      mesh.draw();
      ofPopMatrix();
    };

    virtual void renderEntity(Entity &e)
    {};

    int32 make_particle( float _locx, float _locy, float _velx, float _vely )
    {
      float locx, locy, velx, vely;
      fisica->screen2world(_locx,_locy,locx,locy);
      fisica->screen2world(_velx,_vely,velx,vely);

      uint32 flags = b2_waterParticle | /*b2_springParticle |*/ b2_viscousParticle;

      b2ParticleDef pd;
      pd.flags = flags;
      pd.position.Set( locx, locy );
      pd.velocity.Set( velx, vely );
      pd.color = b2color;
      if ( lifetime > 0.0 )
        pd.lifetime = lifetime; 

      return b2particles->CreateParticle( pd );
    }; 

    b2ParticleSystem* b2_particles()
    {
      return b2particles;
    }; 

  private:

    ComponentMapper<ParticleSystemComponent> ps_m;

    FisicaSystem* fisica;
    b2ParticleSystem* b2particles;

    ofVboMesh mesh;

    //TODO params
    ofColor ofcolor;
    b2ParticleColor b2color;
    float max_count, lifetime, radius, render_size;

    void init()
    {
      mesh.setMode( OF_PRIMITIVE_POINTS );

      max_count = 5000;
      lifetime = 10.0; 
      radius = 6.0;
      render_size = 4.0;
      ofcolor.set( 127, 200, 255 );

      b2color.Set( ofcolor.r, ofcolor.g, ofcolor.b, ofcolor.a );

      b2ParticleSystemDef psd;
      psd.radius = radius / fisica->scale();
      psd.maxCount = max_count;

      /// Reduces relative velocity of viscous particles
      /// Larger values slow down viscous particles more
      psd.viscousStrength = 0.6f; //0.25f;

      /// Change the particle density.
      /// Particle density affects the mass of the particles, which in turn
      /// affects how the particles interact with b2Bodies. Note that the density
      /// does not affect how the particles interact with each other.
      //psd.density = 0.0f; //1.0f;

      /// Pushes particles out of solid particle group
      /// Larger values repulse more
      //psd.ejectionStrength = 0.0f; //0.5f;

      b2particles = fisica->b2world()->CreateParticleSystem( &psd );
    };

};

