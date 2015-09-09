#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "bloqs/Attractor.h"
#include "events/AttractorEvents.h"
#include "PDraw.h"
#define TOTAL_LINES 4

using namespace artemis;

class ParticleAttractorSystem : public ECSsystem 
{ 

  public:

    ParticleAttractorSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<ParticleAttractorComponent>();
      addComponentType<BloqComponent>();
      addComponentType<KnobComponent>();
    };

    virtual void initialize() 
    {
      particle_attractor_m.init( *world );
      bloq_m.init( *world ); 
      knob_m.init( *world );
      
      draw_color = ofColor(242,108,79);
      draw_color_2 = ofColor(37,190,170);
      area_scale_vel = 1.0;
      q_counter=0;
      draw_scale = 0.0;
      
    };

    virtual void removed(Entity &e) 
    { 
      ofNotifyEvent( AttractorEvents::removed, bloq_m.get(e)->bloq->id );
    };

    virtual void added(Entity &e) 
    {
      ofNotifyEvent( AttractorEvents::added, make_attractor(e) );
      ParticleAttractorComponent* attr_data = particle_attractor_m.get(e);

      draw_scale = 0.0;
      draw_vel_scale = 0.0;

    };

    virtual void processEntity(Entity &e) 
    {
      ofNotifyEvent( AttractorEvents::updated, make_attractor(e) );
      ParticleAttractorComponent* attr_data = particle_attractor_m.get(e);

      //  draw
      draw_vel_scale+=(1.0-draw_scale)*.060;
      draw_vel_scale*=.9;
      draw_scale+=draw_vel_scale;
      
    }; 

    virtual void renderEntity(Entity &e)
    {
      RenderComponent* render_data = component<RenderComponent>("output");
      ParticleAttractorComponent* attr_data = particle_attractor_m.get(e);
      
      float r = radius_from_knob(e);
      float t = knob_m.get(e)->value;
      Bloq* bloq = bloq_m.get(e)->bloq;
      ofVec2f& dir = bloq->dir_i; 
      ofVec2f loc( bloq->loc_i ); 
      loc.x *= render_data->width;
      loc.y *= render_data->height;
 
      float knob_r = r*render_data->height;
      
      float norm_scale = ofMap(knob_r,0.0,render_data->height,0.0,1.0);
      float top_scale = norm_scale*draw_scale;
      
      area_scale_vel-=(attr_data->force*.01);
      if(attr_data->force >= 0){
        if(area_scale_vel <= 0.0)
        {
          area_scale_vel = 1.0;
        }
      }else{
        if(area_scale_vel >= 1.0)
        {
          area_scale_vel = 0.0;
        }
      }
      
      float _lerp_color = ofMap(attr_data->force,-10.0,10.0,0.0,1.0);
      ofColor _color = draw_color_2.getLerped(draw_color, _lerp_color);
      
      
      ofPushMatrix();
      ofTranslate(loc.x,loc.y);
      
      
      for(int i=0;i<TOTAL_LINES;i++){
        float _delta = ofMap(i+area_scale_vel,0,TOTAL_LINES,0.0,1.0,true);
        float _s_scale = _delta;
        area_circle.circle(_s_scale*knob_r, _s_scale*(knob_r+attr_data->draw_weight), 10, _color, _color,0,360,true);
      }
      area_circle.circle(knob_r, knob_r+attr_data->draw_weight, 10, _color, _color,0,360,true);
      ofPopMatrix();

    };

  private:

    ComponentMapper<ParticleAttractorComponent> particle_attractor_m;
    ComponentMapper<BloqComponent> bloq_m;  
    ComponentMapper<KnobComponent> knob_m;
  
  
    PDraw area_circle;
    PDraw direction_circle;
    ofColor draw_color;
    ofColor draw_color_2;
    int   q_counter;
    float area_scale_vel;
    float  draw_scale;
    float  draw_vel_scale;
  
    Attractor& make_attractor(Entity &e)
    {
      ParticleAttractorComponent* attr_data = particle_attractor_m.get(e);
      Bloq* bloq = bloq_m.get(e)->bloq;
      static Attractor attr;
      attr.id = bloq->id;
      attr.loc.set( bloq->loc );
      attr.force = attr_data->force;
      attr.radius = radius_from_knob(e);
      return attr;
    };

    float radius_from_knob(Entity &e) 
    {
      float t = knob_m.get(e)->value;
      float r = particle_attractor_m.get(e)->radius;
      return ofMap(t, 0., 1., 0.01, r);
    };

};

