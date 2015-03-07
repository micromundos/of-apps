#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ecs/Components.h"

using namespace artemis;

class KnobSystem : public ECSsystem 
{ 

  public:

    KnobSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<KnobComponent>();
      addComponentType<BloqComponent>();
    };

    virtual void initialize() 
    {
      knob_m.init( *world );
      bloq_m.init( *world );
    };

    virtual void added(Entity &e) 
    {
      //knob_m.get(e)->data;
    };

    virtual void processEntity(Entity &e) 
    {
      Bloq* bloq = bloq_m.get(e)->bloq;
      float ang = bloq->radians / TWO_PI;

      //http://stackoverflow.com/questions/13097005/easing-functions-for-bell-curves
      knob_m.get(e)->value = ((sin( TWO_PI * (ang - 0.25f)) + 1) * 0.5);

      //ofLogNotice("KnobSystem") << "ang: " << ang << ", value: " << knob_m.get(e)->value;
    }; 

    virtual void render_entity(Entity &e)
    {};

  private:

    ComponentMapper<KnobComponent> knob_m;
    ComponentMapper<BloqComponent> bloq_m;

};

