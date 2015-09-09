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
      
      
      
      area_circle.circle(100, 120, 10, ofColor(242,108,79), ofColor(242,108,79));
      
      area_scale_vel = 1.0;
      q_counter=0;
      draw_scale = 0.0;
      for(int i=0;i<5;i++)
        scale_qeue[i] = 1.0;
      
    };

    virtual void removed(Entity &e) 
    { 
      ofNotifyEvent( AttractorEvents::removed, bloq_m.get(e)->bloq->id );
    };

    virtual void added(Entity &e) 
    {
      ofNotifyEvent( AttractorEvents::added, make_attractor(e) );
      draw_scale = 0.0;
      draw_vel_scale = 0.0;
      
    };

    virtual void processEntity(Entity &e) 
    {
      ofNotifyEvent( AttractorEvents::updated, make_attractor(e) );
      
      
      //  draw
      draw_vel_scale+=(1.0-draw_scale)*.060;
      draw_vel_scale*=.9;
      draw_scale+=draw_vel_scale;
      
    }; 

    virtual void renderEntity(Entity &e)
    {
      RenderComponent* render_data = component<RenderComponent>("output");

      float r = radius_from_knob(e);
      float t = knob_m.get(e)->value;
      Bloq* bloq = bloq_m.get(e)->bloq;
      ofVec2f& dir = bloq->dir_i; 
      ofVec2f loc( bloq->loc_i ); 
      loc.x *= render_data->width;
      loc.y *= render_data->height;
 
      float top_scale = (.2+(t)*2.0)*draw_scale;
      area_scale_vel-=.02;
      if(area_scale_vel <= 0.0)
      {
        area_scale_vel = 1.0;
      }
      
      for(int i=4;i>0;i--)
      {
        scale_qeue[i] = scale_qeue[i-1];
      }
      scale_qeue[0] = area_scale_vel;
      q_counter++;
      

      
      ofPushMatrix();
      ofTranslate(loc.x,loc.y);
      ofRotate(ofRadToDeg(bloq->radians_i)+ofGetFrameNum()*.6);
      for(int i=0;i<5;i++){
        float _delta = ofMap(i+scale_qeue[i],0,5,0.0,1.0,true);
        float _s_scale = _delta;
        ofPushMatrix();
        ofScale(_s_scale,_s_scale);
        ofScale(top_scale,top_scale);
        area_circle.draw();
        ofPopMatrix();
      }
      
      ofScale(top_scale,top_scale);
      area_circle.draw();
      ofPopMatrix();

    };

  private:

    ComponentMapper<ParticleAttractorComponent> particle_attractor_m;
    ComponentMapper<BloqComponent> bloq_m;  
    ComponentMapper<KnobComponent> knob_m;
  
  
    PDraw area_circle;
    PDraw direction_circle;
    ofColor draw_color;
    float scale_qeue[5];
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

