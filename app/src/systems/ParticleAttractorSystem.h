#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "bloqs/Attractor.h"
#include "events/AttractorEvents.h"
#include "PDraw.h"

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
      
    };

    virtual void removed(Entity &e) 
    { 
      ofNotifyEvent( AttractorEvents::removed, bloq_m.get(e)->bloq->id );
    };

    virtual void added(Entity &e) 
    {
      if ( !check_bloq(e) ) return;

      ofNotifyEvent( AttractorEvents::added, make_attractor(e) );
      ParticleAttractorComponent* attr_data = particle_attractor_m.get(e);

      
      
      attr_data->draw_color = ofColor(242,108,79);
      attr_data->draw_color_2 = ofColor(255,100,0);
      attr_data->area_scale_vel = 1.0;
      attr_data->q_counter=0;
      attr_data->draw_scale = 0.0;
      attr_data->total_lines = 3;
      attr_data->circle_res = 30;

      
      attr_data->draw_scale = 0.0;
      attr_data->draw_vel_scale = 0.0;
    };

    virtual void processEntity(Entity &e) 
    {
      if ( !check_bloq(e) ) return;

      ofNotifyEvent( AttractorEvents::updated, make_attractor(e) );
      ParticleAttractorComponent* attr_data = particle_attractor_m.get(e);

      //  draw
      attr_data->draw_vel_scale+=(1.0-attr_data->draw_scale)*.060;
      attr_data->draw_vel_scale*=.9;
      attr_data->draw_scale+=attr_data->draw_vel_scale;
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
      float top_scale = norm_scale*attr_data->draw_scale;
      
      attr_data->area_scale_vel-=(attr_data->force*.01);
      if(attr_data->force >= 0){
        if(attr_data->area_scale_vel <= 0.0)
        {
          attr_data->area_scale_vel = 1.0;
        }
      }else{
        if(attr_data->area_scale_vel >= 1.0)
        {
          attr_data->area_scale_vel = 0.0;
        }
      }
      
      float _lerp_color = ofMap(attr_data->force,-10.0,10.0,0.0,1.0);
      ofColor _color = attr_data->draw_color_2.getLerped(attr_data->draw_color, _lerp_color);
      _color.a = 150;
      

      ofPushMatrix();
      ofTranslate(loc.x,loc.y);
      
      
      for(int i=0;i<attr_data->total_lines;i++){
        float _delta = ofMap(i+attr_data->area_scale_vel,0,attr_data->total_lines,0.0,1.0,true);
        float _s_scale = _delta;
        attr_data->area_circle.circle(_s_scale*knob_r, _s_scale*(knob_r+attr_data->draw_weight), attr_data->circle_res, _color, _color,0,360,true);
      }
      attr_data->area_circle.circle(knob_r, knob_r+attr_data->draw_weight, attr_data->circle_res, _color, _color,0,360,true);
      ofPopMatrix();
      /*
      ofPushStyle();
      ofEnableAlphaBlending();
      ofSetLineWidth( 6 );
      ofSetColor( ofColor( ofColor::gold.getLerped( ofColor::crimson, t ), 100 ) );
      ofCircle( loc, 20 );
      ofSetColor( ofColor::orange );
      ofLine( loc, loc + dir * 20 );
      ofDisableAlphaBlending();
      ofPopStyle();
      ofPushStyle();
      ofNoFill();
      ofSetColor( ofColor::orange );
      ofCircle( loc, r * render_data->height );
      ofPopStyle();*/
    };

  private:

    ComponentMapper<ParticleAttractorComponent> particle_attractor_m;
    ComponentMapper<BloqComponent> bloq_m;  
    ComponentMapper<KnobComponent> knob_m;
  
  

    Attractor& make_attractor(Entity &e)
    {
      ParticleAttractorComponent* attr_data = particle_attractor_m.get(e);
      Bloq* bloq = bloq_m.get(e)->bloq; 
      string id = bloq->id;
      static Attractor attr;
      attr.id = id;
      attr.loc.set( bloq->loc );
      attr.force = attr_data->force;
      attr.radius = radius_from_knob(e);
      return attr;
    };

    bool check_bloq(Entity &e)
    {
      if (bloq_m.get(e)->bloq == NULL)
      {
        ofLogError("ParticleAttractorSystem")
          << "entity [" << e.getId() << "] "
          << "bloq is NULL !@#$%ˆ&*";
        return false;
      }
      return true;
    }

    float radius_from_knob(Entity &e) 
    {
      float t = knob_m.get(e)->value;
      //Bloq* bloq = bloq_m.get(e)->bloq;
      //float t = bloq->radians_i/TWO_PI;
      float r = particle_attractor_m.get(e)->radius;
      return ofMap(t, 0., 1., 0.01, r);
    };

};

