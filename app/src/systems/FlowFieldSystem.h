#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

using namespace artemis;

/*
 * Integrates all flow fields
 */

class FlowFieldSystem : public ECSsystem 
{ 

  public:

    FlowFieldSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<FlowFieldComponent>();
      addComponentType<FlowFieldContainerComponent>();
      addComponentType<FlowFieldGradientComponent>();
      addComponentType<DepthComponent>();
    };

    virtual void initialize() 
    {
      ff_m.init( *world );
      ff_container_m.init( *world );
      ff_gradient_m.init( *world );
      depth_m.init( *world );
    };

    virtual void added(Entity &e) 
    {
      float scale = ff_m.get(e)->scale;
      DepthComponent* depth_data = depth_m.get(e);
      int w = depth_data->width * scale;
      int h = depth_data->height * scale;

      flowfield(e)
        .init("glsl/flowfields/flowfield.frag",w,h);
    }; 

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);
      if ( ! depth_data->dirty ) return;

      TS_START("FlowFieldSystem");

      flowfield(e)

        .set( "flowfield_0", ff_container_m
            .get(e)->flowfield().get() )

        .set( "flowfield_1", ff_gradient_m
            .get(e)->flowfield().get() )

        .update();

      if ( ff_m.get(e)->render )
        flowfield(e).update_debug();

      TS_STOP("FlowFieldSystem");
    }; 

    virtual void renderEntity(Entity &e)
    {
      TS_START("FlowFieldSystem render");

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      if ( ff_m.get(e)->render )
        flowfield(e).render_debug(0,0,rw,rh);

      TS_STOP("FlowFieldSystem render");
    };

  private:

    ComponentMapper<FlowFieldComponent> ff_m;
    ComponentMapper<FlowFieldContainerComponent> ff_container_m;
    ComponentMapper<FlowFieldGradientComponent> ff_gradient_m;
    ComponentMapper<DepthComponent> depth_m; 

    gpgpu::Process& flowfield(Entity &e)
    {
      return ff_m.get(e)->flowfield();
    };

};

