#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "bloqs/Attractor.h"
#include "events/AttractorEvents.h"

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
      ofNotifyEvent( AttractorEvents::added, make_attractor(e) );
    };

    virtual void processEntity(Entity &e) 
    {
      ofNotifyEvent( AttractorEvents::updated, make_attractor(e) );
    }; 

    virtual void renderEntity(Entity &e)
    {
      RenderComponent* render_data = component<RenderComponent>("output");

      float r = radius_from_knob(e);
      float t = knob_m.get(e)->value;
      Bloq* bloq = bloq_m.get(e)->bloq;
      ofVec2f& dir = bloq->dir; 
      ofVec2f loc( bloq->loc ); 
      loc.x *= render_data->width;
      loc.y *= render_data->height;

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
      ofPopStyle();
    };

  private:

    ComponentMapper<ParticleAttractorComponent> particle_attractor_m;
    ComponentMapper<BloqComponent> bloq_m;  
    ComponentMapper<KnobComponent> knob_m;

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

