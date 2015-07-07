#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

using namespace artemis;

class TimeBenderSystem : public ECSsystem 
{ 

  public:

    TimeBenderSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<TimeBenderComponent>();
      addComponentType<BloqComponent>();
      addComponentType<KnobComponent>();
    };

    virtual void initialize() 
    {
      time_bender_m.init( *world );
      bloq_m.init( *world );
      knob_m.init( *world );

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
      float t = knob_m.get(e)->value;

      float fps = ofMap( t, 0.0f, 1.0f, initial_fps_fisica, initial_fps_fisica * coef, true );

      //ofLogNotice("TimeBenderSystem") << "t: " << t << ", fps: " << fps << ", dt: " << (1.0f/fps) << ", coef: " << coef;

      fisica->fps( fps );
      //ofSetFrameRate( fps_of );
    };

    virtual void renderEntity(Entity &e)
    {};

  private:

    ComponentMapper<TimeBenderComponent> time_bender_m;
    ComponentMapper<BloqComponent> bloq_m;
    ComponentMapper<KnobComponent> knob_m;

    FisicaSystem* fisica;
    float initial_fps_fisica;
    //float initial_fps_of;
};

