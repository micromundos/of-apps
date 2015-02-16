#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ecs/Components.h"

using namespace artemis;

class TimeBenderSystem : public ECSsystem 
{ 

  public:

    TimeBenderSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<TimeBenderComponent>();
      addComponentType<BloqComponent>();
    };

    virtual void initialize() 
    {
      time_bender_m.init( *world );
      bloq_m.init( *world );

      fisica = require_system<FisicaSystem>();

      initial_fps_fisica = fisica->fps();
      //initial_fps_of = ofGetFrameRate();
    };

    virtual void added(Entity &e) 
    {};

    virtual void removed(Entity &e) 
    {
      //ofSetFrameRate( initial_fps_of );
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("TimeBenderSystem") << "process entity " << e.getId();

      float coef = time_bender_m.get(e)->coef;

      Bloq* bloq = bloq_m.get(e)->bloq;
      float ang = bloq->radians / TWO_PI;

      //http://stackoverflow.com/questions/13097005/easing-functions-for-bell-curves
      float t = ((sin( TWO_PI * (ang - 0.25f)) + 1) * 0.5);
      float fps = ofMap( t, 0.0f, 1.0f, initial_fps_fisica, initial_fps_fisica * coef, true );

      ofLogNotice("TimeBenderSystem") << "ang: " << ang << ", t: " << t << ", fps: " << fps << ", dt: " << (1.0f/fps) << ", coef: " << coef;

      fisica->fps( fps );
      //ofSetFrameRate( fps_of );
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      int len = bag.getCount();
      for ( int i = 0; i < len; i++ )
        processEntity( *bag.get(i) );
    };

    virtual void render_entity(Entity &e)
    {};

  private:

    ComponentMapper<TimeBenderComponent> time_bender_m;
    ComponentMapper<BloqComponent> bloq_m;

    FisicaSystem* fisica;
    float initial_fps_fisica;
    //float initial_fps_of;
};

