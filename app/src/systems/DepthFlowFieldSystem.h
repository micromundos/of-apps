#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ecs/Components.h"

using namespace artemis;

class DepthFlowFieldSystem : public ECSsystem 
{ 

  public:

    DepthFlowFieldSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<DepthComponent>();
      addComponentType<FlowFieldComponent>();
    };

    virtual void initialize() 
    {
      depth_m.init( *world );
      flowfield_m.init( *world );
    };

    virtual void processEntity(Entity &e) 
    { 
      DepthComponent* depth = depth_m.get(e);
      if ( ! depth->dirty ) return;
      uint16_t *depth_pix_mm = depth->depth_pix_mm;
      //TODO process depth, generate ff
      //flowfield_m.get(e)->update(xxx);
    };

    virtual void render_entity(Entity &e)
    {};

  private:

    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<FlowFieldComponent> flowfield_m;

};

