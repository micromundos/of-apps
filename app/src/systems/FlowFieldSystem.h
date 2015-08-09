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
    };

    virtual void initialize() 
    {
      ff_m.init( *world );
      ff_container_m.init( *world );
      ff_gradient_m.init( *world );
      entity = NULL;
      depth_data = NULL;
      render_data = NULL;
    };

    virtual void removed(Entity &e) 
    {
      output(e).off( "update", this, &FlowFieldSystem::update_flowfield ); 
      entity = NULL;
      depth_data = NULL;
      render_data = NULL;
    };

    virtual void added(Entity &e) 
    {
      if ( entity != NULL )
      {
        ofLogWarning("FlowFieldSystem")
          << "entity already added";
        return;
      }
      entity = &e;

      depth_data = require_component<DepthComponent>("input");
      render_data = require_component<RenderComponent>("output");

      float scale = ff_m.get(e)->scale;
      int w = depth_data->width * scale;
      int h = depth_data->height * scale;

      output(e)
        .init("glsl/flowfields/flowfield.frag",w,h)
        .on( "update", this, &FlowFieldSystem::update_flowfield );
    }; 

    virtual void processEntity(Entity &e) 
    {
      if ( ! depth_data->dirty ) return;

      TS_START("FlowFieldSystem");

      output(e)
        .set( "flowfield_0", ff_container_m
            .get(e)->flowfield().get() )
        .set( "flowfield_1", ff_gradient_m
            .get(e)->flowfield().get() )
        .update()
        .update_debug( ff_m.get(e)->render );

      TS_STOP("FlowFieldSystem");
    }; 

    virtual void renderEntity(Entity &e)
    {
      if ( !ff_m.get(e)->render ) return;

      TS_START("FlowFieldSystem render");

      int rw = render_data->width;
      int rh = render_data->height;

      output(e).render_debug(0,0,rw,rh);

      TS_STOP("FlowFieldSystem render");
    };

  private:

    Entity* entity;
    DepthComponent* depth_data;
    RenderComponent* render_data;

    ComponentMapper<FlowFieldComponent> ff_m;
    ComponentMapper<FlowFieldContainerComponent> ff_container_m;
    ComponentMapper<FlowFieldGradientComponent> ff_gradient_m;

    void update_flowfield( ofShader& shader )
    {
      FlowFieldComponent* ff_data = ff_m.get( *entity ); 
      shader.setUniform1f( "weight_0", ff_data->weight_0 );
      shader.setUniform1f( "weight_1", ff_data->weight_1 );
    };

    gpgpu::Process& output(Entity &e)
    {
      return ff_m.get(e)->flowfield();
    };

};

