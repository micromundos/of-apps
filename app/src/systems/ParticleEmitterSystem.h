#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "Systems.h"
#include "PDraw.h"

using namespace artemis;

class ParticleEmitterSystem : public ECSsystem 
{ 

  public:

    ParticleEmitterSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<ParticleEmitterComponent>();
      addComponentType<BloqComponent>();
    };

    virtual void initialize() 
    {
      emitter_m.init( *world );
      bloq_m.init( *world );

      FisicaSystem* fisica = system<FisicaSystem>();

      initial_fps_fisica = fisica->fps();
      emit_remainder = 0.0f;

        };
  
    virtual void removed(Entity &e)
    {
      ParticleEmitterComponent* emitter_data = emitter_m.get(e);

      emitter_data->draw_scale_d = 0.0;
    };
    
    virtual void added(Entity &e)
    {
      ParticleEmitterComponent* emitter_data = emitter_m.get(e);
      
      emitter_data->circle_color = ofColor::turquoise;
      emitter_data->direction_color = ofColor::mediumTurquoise;
      emitter_data->direction_color_l = ofColor::powderBlue;
      emitter_data->draw_scale_d = 1.0;
      emitter_data->draw_scale =0.0;
      emitter_data->draw_vel_scale = 0.0;
      emitter_data->draw_resolution = 3;
      emitter_data->draw_inited = false;


    };

    virtual void processEntity(Entity &e) 
    { 
      Bloq* bloq = bloq_m.get(e)->bloq;

      ParticleEmitterComponent* emitter_data = emitter_m.get(e);
      RenderComponent* render_data = component<RenderComponent>("output");

      // How many (fractional) particles should we have emitted this frame?
      float	dt = (1.0f / initial_fps_fisica);
      //float	dt = (1.0f / fisica->fps());
      //float	dt = (1.0f / ofGetFrameRate());
      float rate = emitter_data->rate;
      emit_remainder += rate * dt;

      // Keep emitting particles on this frame until we only have a fractional particle left.
      while (emit_remainder > 1.0f) 
      {
        emit_remainder -= 1.0f;
        emit( bloq, emitter_data, render_data );
      }
      
      //  draw
      emitter_data->draw_vel_scale+=(emitter_data->draw_scale_d-emitter_data->draw_scale)*.060;
      emitter_data->draw_vel_scale*=.9;
      emitter_data->draw_scale+=emitter_data->draw_vel_scale;
      
    };
  
  
  
  
    virtual void renderEntity(Entity &e)
    {
      RenderComponent* render_data = component<RenderComponent>("output");
      ParticleEmitterComponent* emitter_data = emitter_m.get(e);

      Bloq* bloq = bloq_m.get(e)->bloq;
        
      ofVec2f& dir = bloq->dir_i;
      ofVec2f loc( bloq->loc_i);
      loc.x *= render_data->width;
      loc.y *= render_data->height;
      ofPushStyle();

      ofPushMatrix();
      ofTranslate(loc.x,loc.y);
      ofScale(emitter_data->draw_scale,emitter_data->draw_scale);
      ofRotate(ofRadToDeg(-bloq->radians_i)-90);
      emitter_data->draw_circle.circle(emitter_data->draw_radius,emitter_data->draw_radius+emitter_data->draw_weight,emitter_data->draw_resolution,emitter_data->circle_color,emitter_data->circle_color,0,360,true);
      
      ofColor _color = ofColor::darkTurquoise;
      _color.a = .2+abs(sin(ofGetFrameNum()*.05))*255.0;
      
      float _new_radius =emitter_data->draw_radius+(emitter_data->draw_weight*2.0);
      emitter_data->draw_circle.circle(_new_radius,_new_radius+emitter_data->draw_weight,emitter_data->draw_resolution,_color,_color,0,360,true);
      

      ofPopMatrix();
      ofPopStyle();
    };

  private:

    ComponentMapper<ParticleEmitterComponent> emitter_m;
    ComponentMapper<BloqComponent> bloq_m;
  

    float initial_fps_fisica;
    float emit_remainder;
  /*
    PDraw  draw_circle;
    PDraw  draw_direction,draw_direction_3;
    ofColor circle_color;
    ofColor direction_color;
    ofColor direction_color_l;
    int    draw_resolution;
    float  draw_scale;
    float  draw_vel_scale;
    float  draw_scale_d;
    bool   draw_inited;
  */
  
    void emit( Bloq* bloq, ParticleEmitterComponent* emitter_data, RenderComponent* render_data )
    {
      ParticleSystem* ps = system<ParticleSystem>();

      //ofxBox2dParticleSystem* ofps = ps->of_particles();
      b2ParticleSystem* b2ps = ps->b2_particles();

      ofVec2f screen_loc( bloq->loc.x * render_data->width, bloq->loc.y * render_data->height );

      //int32 pidx = ofps->createParticle( screen_loc.x, screen_loc.y, 0, 0 );
      
      
      
      int32 pidx = ps->make_particle( screen_loc.x, screen_loc.y, 0, 0 ,bloq->id == "1" ? ofColor(0,140,140) : ofColor(140,0,140));

      float force_m = emitter_data->force;

      //ofVec2f force = bloq->dir * force_m;
      //ofps->applyForce( pidx, force );
      b2Vec2 force( bloq->dir.x, bloq->dir.y );
      force *= force_m;
      b2ps->ParticleApplyForce( pidx, force );
    };

};

