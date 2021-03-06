#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "Systems.h"

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
      entity = NULL;
      par_texture.setUseTexture(true);
      par_texture.loadImage("assets/par2.png");
    };

    virtual void removed(Entity &e) 
    { 
      entity = NULL;
    };

    virtual void added(Entity &e) 
    {
      if ( entity != NULL )
      {
        ofLogWarning("ParticleSystem")
          << "singleton entity already added";
        return;
      } 
      entity = &e;

      mesh.setMode( OF_PRIMITIVE_POINTS );

      ParticleSystemComponent* ps_data = ps_m.get(e);

      ofcolor.set( 127, 200, 255 );

      b2color.Set( ofcolor.r, ofcolor.g, ofcolor.b, ofcolor.a );

      b2ParticleSystemDef psd;
      psd.radius = ps_data->radius/fisica->scale();
      psd.maxCount = ps_data->max_particles;

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

    virtual void processEntity(Entity &e)
    {
      TS_START("ParticleSystem"); 

      ParticleSystemComponent* ps_data = ps_m.get(e);

      //limit particles speed

      float max_speed = ps_data->max_speed;

      if ( max_speed == 0.0 )
        return;

      int32 n = b2particles->GetParticleCount();
      b2Vec2 *vels = b2particles->GetVelocityBuffer(); 
      for (int i = 0; i < n; i++)
      {
        b2Vec2& vel = vels[i];
        float len = vel.Normalize();
        vel *= len > max_speed ? max_speed : len;
        //cout << i << ": vel len " << len << ", max " << max_speed << endl;
      }

      TS_STOP("ParticleSystem"); 
    }; 

    virtual void renderEntity(Entity &e)
    {
      ParticleSystemComponent* ps_data = ps_m.get(e);

      mesh.clear();

      int32 n = b2particles->GetParticleCount();
      b2Vec2 *locs = b2particles->GetPositionBuffer();
      b2ParticleColor *cols = b2particles->GetColorBuffer();

      ofVec2f loc;
      ofFloatColor col;
     // ofEnableBlendMode(OF_BLENDMODE_ADD);
      ofPushStyle();
      ofEnableAlphaBlending();
      for ( int i = 0; i < n; i++ )
      {
        loc.set( locs[i].x, locs[i].y );
        loc*=fisica->scale();
        ofColor _col = ofColor(cols[i].r,cols[i].g,cols[i].b);
        ofColor _color2 = _col.getLerped(ofColor(255,255,255),ofNoise(locs[i].x,locs[i].y,ofGetFrameNum()*.05));
        ofSetColor(cols[i].r,cols[i].g,cols[i].b,255.0);
        ofPushMatrix();
        ofTranslate(loc.x,loc.y);
        ofScale(ps_data->render_size ,ps_data->render_size );
        ofPushStyle();
      //  ofSetColor(cols[i].r,cols[i].g,cols[i].b,255.0);
        ofSetColor(_color2);
        par_texture.draw(-par_texture.getWidth()/2,-par_texture.getHeight()/2);
        ofPopStyle();
        ofPopMatrix();

        //loc.set( locs[i].x, locs[i].y );
        //col.set( cols[i].r / 255.0, cols[i].g / 255.0, cols[i].b / 255.0 );
        //mesh.addVertex( loc );
        //mesh.addColor( col );
      }
      ofDisableAlphaBlending();
      ofPopStyle();
      //ofDisableBlendMode();

      //ofPushStyle();
      //ofSetColor(255);
      //ofSetLineWidth(0.1);
      //glPointSize( ps_data->render_size );
      //ofPushMatrix();
      //ofScale( fisica->scale(), fisica->scale() );
      //mesh.draw();
      //ofPopMatrix();
      //ofPopStyle();
    };

    int32 make_particle( float _locx, float _locy, float _velx, float _vely ,ofColor _color )
    {
      if ( entity == NULL )
        return -1;

      ParticleSystemComponent* ps_data = ps_m.get( *entity );

      float locx, locy, velx, vely;
      fisica->screen2world(_locx,_locy,locx,locy);
      fisica->screen2world(_velx,_vely,velx,vely);

      uint32 flags = b2_waterParticle | /*b2_springParticle |*/ b2_viscousParticle;

      b2ParticleDef pd;
      pd.flags = flags;
      pd.position.Set( locx, locy );
      pd.velocity.Set( velx, vely );
      b2ParticleColor p_color;
      p_color.Set(_color.r,_color.g,_color.b,255.0);
      pd.color = p_color;
      if ( ps_data->lifetime > 0.0 )
        pd.lifetime = ps_data->lifetime; 

      return b2particles->CreateParticle( pd );
    }; 

    b2ParticleSystem* b2_particles()
    {
      return b2particles;
    }; 

  private:

    ComponentMapper<ParticleSystemComponent> ps_m;
  
    Entity* entity;
    b2ParticleSystem* b2particles;
    FisicaSystem* fisica;
    ofImage   par_texture;
    ofVboMesh mesh;

    ofColor ofcolor;
    b2ParticleColor b2color;
};

