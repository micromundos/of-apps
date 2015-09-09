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

      fisica = require_system<FisicaSystem>();
      ps = require_system<ParticleSystem>();

      initial_fps_fisica = fisica->fps();
      emit_remainder = 0.0f;
      //  draw
      
      circle_color = ofColor(0,71,89);
      direction_color = ofColor(156,196,228);
      direction_color_l = ofColor(233,242,249);
      
      ofEnableSmoothing();
      ofSetCircleResolution(128);
      draw_scale = draw_scale_d = 0.0;
      draw_vel_scale = 0.0;
      draw_resolution = 40;
      draw_inited = false;
      
      
      
    };
  
    virtual void removed(Entity &e)
    {
      draw_scale_d = 0.0;
    };
    
    virtual void added(Entity &e)
    {
      ParticleEmitterComponent* emitter_data = emitter_m.get(e);

      draw_scale = 0.0;
      draw_vel_scale = 0.0;
      draw_scale_d = 1.0;

      
    };

    // entity: bloq
    virtual void processEntity(Entity &e) 
    { 
      //ofLogNotice("ParticleEmitterSystem") << "process entity " << e.getId();

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
      draw_vel_scale+=(draw_scale_d-draw_scale)*.060;
      draw_vel_scale*=.9;
      draw_scale+=draw_vel_scale;
      
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

      ofPushMatrix();
      ofTranslate(loc.x,loc.y);
      ofScale(draw_scale,draw_scale);
      ofRotate(ofRadToDeg(-bloq->radians_i)-90);
      draw_circle.circle(emitter_data->draw_radius,emitter_data->draw_radius+emitter_data->draw_weight,draw_resolution,circle_color,circle_color,10,355,true);
      ofPushStyle();
      ofNoFill();
      ofSetColor(direction_color);
      float _line_rad = (2.0*emitter_data->draw_radius)+(emitter_data->draw_weight*3.0);
      ofEllipse(0,0,_line_rad,_line_rad);
      ofPopStyle();
      
      draw_direction_3.circle(emitter_data->draw_radius,emitter_data->draw_radius+emitter_data->draw_weight,draw_resolution,direction_color_l,direction_color_l,10,20,true);
      
      draw_direction_3.circle(emitter_data->draw_radius,emitter_data->draw_radius+emitter_data->draw_weight,draw_resolution,direction_color_l,direction_color_l,336,346
                              ,true);
      
      ofColor _color = ofColor(233,242,249,(ofGetFrameNum()*.03)*255.0);
      draw_direction_3.circle(emitter_data->draw_radius,emitter_data->draw_radius+emitter_data->draw_weight,draw_resolution,_color,_color,10,20,true);
    
      draw_direction_3.circle(emitter_data->draw_radius,emitter_data->draw_radius+emitter_data->draw_weight,draw_resolution,_color,_color,336,346
                              ,true);
      ofPopMatrix();
      
    };

  private:

    FisicaSystem* fisica;
    ParticleSystem* ps;

    ComponentMapper<ParticleEmitterComponent> emitter_m;
    ComponentMapper<BloqComponent> bloq_m;
  

    float initial_fps_fisica;
    float emit_remainder;
  
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
  
  
    void emit( Bloq* bloq, ParticleEmitterComponent* emitter_data, RenderComponent* render_data )
    {
      //ofxBox2dParticleSystem* ofps = ps->of_particles();
      b2ParticleSystem* b2ps = ps->b2_particles();

      ofVec2f screen_loc( bloq->loc.x * render_data->width, bloq->loc.y * render_data->height );

      //int32 pidx = ofps->createParticle( screen_loc.x, screen_loc.y, 0, 0 );
      int32 pidx = ps->make_particle( screen_loc.x, screen_loc.y, 0, 0 );

      float force_m = emitter_data->force;

      //ofVec2f force = bloq->dir * force_m;
      //ofps->applyForce( pidx, force );
      b2Vec2 force( bloq->dir.x, bloq->dir.y );
      force *= force_m;
      b2ps->ParticleApplyForce( pidx, force );
    };

};

