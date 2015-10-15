#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "bloqs/Bloq.h"

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
      KnobComponent* knob_data = knob_m.get(e);
      Bloq* bloq = bloq_m.get(e)->bloq;

      float ang = bloq->radians_i / TWO_PI;

      //http://stackoverflow.com/questions/13097005/easing-functions-for-bell-curves
      knob_data->value = ((sin( TWO_PI * (ang - 0.25f)) + 1) * 0.5);

      //ofLogNotice("KnobSystem") << "bloq angle: " << ang << ", knob value: " << knob_data->value;
    }; 

    virtual void renderEntity(Entity &e)
    {};

  private:

    ComponentMapper<KnobComponent> knob_m;
    ComponentMapper<BloqComponent> bloq_m;

};

