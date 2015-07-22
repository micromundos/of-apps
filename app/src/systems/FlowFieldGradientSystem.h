#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

using namespace artemis;

class FlowFieldGradientSystem : public ECSsystem 
{ 

  public:

    FlowFieldGradientSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<FlowFieldGradientComponent>();
      addComponentType<FlowFieldComponent>();
      addComponentType<DepthComponent>();
      addComponentType<DepthProcessingComponent>();
    };

    virtual void initialize() 
    {
      flow_field_gradient_m.init( *world );
      flow_field_m.init( *world );
      depth_m.init( *world );
      depth_processing_m.init( *world );
      entity = NULL;
    };

    virtual void added(Entity &e) 
    {
      if ( entity != NULL )
      {
        ofLogWarning("FlowFieldGradientSystem")
          << "entity already added";
        return;
      }
      entity = &e;

      float scale = flow_field_m.get(e)->scale;
      DepthComponent* depth_data = depth_m.get(e);
      int w = depth_data->width * scale;
      int h = depth_data->height * scale;

      flowfield(e)
        .init("glsl/flowfields/flowfield_gradient.frag",w,h)
        .on( "update", this, &FlowFieldGradientSystem::update_flowfield );
    }; 

    virtual void removed(Entity &e) 
    {
      flowfield(e).off( "update", this, &FlowFieldGradientSystem::update_flowfield ); 
      entity = NULL;
    };

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);
      if ( ! depth_data->dirty ) return;

      TS_START("FlowFieldGradientSystem");

      FlowFieldGradientComponent* ff_data = flow_field_gradient_m.get(e); 

      flowfield(e)
        .set( "height_map", surfaces(e).get() )
        .update();

      if ( ff_data->render )
        flowfield(e).update_debug();

      TS_STOP("FlowFieldGradientSystem");
    }; 

    virtual void renderEntity(Entity &e)
    {
      TS_START("FlowFieldGradientSystem render"); 

      FlowFieldGradientComponent* ff_data = flow_field_gradient_m.get(e);  

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      if ( ff_data->render )
        flowfield(e).render_debug(0,0,rw,rh);
        //flowfield(e).get().draw(0,0,rw,rh);

      TS_STOP("FlowFieldGradientSystem render");
    };

  private:

    Entity* entity;

    ComponentMapper<FlowFieldGradientComponent> flow_field_gradient_m;
    ComponentMapper<FlowFieldComponent> flow_field_m;
    ComponentMapper<DepthComponent> depth_m; 
    ComponentMapper<DepthProcessingComponent> depth_processing_m;

    void update_flowfield( ofShader& shader )
    {
      FlowFieldGradientComponent* ff_data = flow_field_gradient_m.get( *entity ); 

      shader.setUniform1f( "force_weight_min", ff_data->force_weight_min );
      shader.setUniform1f( "force_weight_max", ff_data->force_weight_max * ff_data->force_weight_max_m );
    };

    gpgpu::Process& flowfield(Entity &e)
    {
      return flow_field_gradient_m.get(e)->flowfield();
    };

    gpgpu::Process& height_map(Entity &e)
    {
      return depth_processing_m.get(e)->height_map();
    };

    gpgpu::Process& surfaces(Entity &e)
    {
      return depth_processing_m.get(e)->surfaces();
    };

};

