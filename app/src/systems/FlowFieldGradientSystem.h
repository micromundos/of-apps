#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "shaders/gaussian.h"

using namespace artemis;

class FlowFieldGradientSystem : public ECSsystem 
{ 

  public:

    FlowFieldGradientSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<FlowFieldGradientComponent>();
      addComponentType<FlowFieldComponent>();
    };

    virtual void initialize() 
    {
      ff_gradient_m.init( *world );
      ff_m.init( *world );
      entity = NULL;
      depth_data = NULL;
      depth_proc_data = NULL;
      render_data = NULL;
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

      depth_data = require_component<DepthComponent>("input");
      depth_proc_data = require_component<DepthProcessingComponent>("input");
      render_data = require_component<RenderComponent>("output");

      float scale = ff_m.get(e)->scale;
      int w = depth_data->width * scale;
      int h = depth_data->height * scale;

      gaussian
        .init( w, h )
        .on( "update", this, &FlowFieldGradientSystem::update_gaussian );

      output(e)
        .init("glsl/flowfields/flowfield_gradient.frag",w,h)
        .on( "update", this, &FlowFieldGradientSystem::update_flowfield );
    }; 

    virtual void removed(Entity &e) 
    {
      output(e).off( "update", this, &FlowFieldGradientSystem::update_flowfield ); 
      gaussian.off( "update", this, &FlowFieldGradientSystem::update_gaussian );

      entity = NULL;
      depth_data = NULL;
      depth_proc_data = NULL;
      render_data = NULL;
    };

    virtual void processEntity(Entity &e) 
    {
      if ( ! depth_data->dirty ) return;

      TS_START("FlowFieldGradientSystem");

      FlowFieldGradientComponent* ff_data = ff_gradient_m.get(e); 

      gaussian
        .set( "data", input(e).get() )
        .update( 2 ); //horiz + vert

      output(e)
        .set( "height_map", gaussian.get() )
        //.set( "height_map", input(e).get() )
        .update()
        .update_debug( ff_data->render );

      TS_STOP("FlowFieldGradientSystem");
    }; 

    virtual void renderEntity(Entity &e)
    {
      if ( !ff_gradient_m.get(e)->render )
        return;

      TS_START("FlowFieldGradientSystem render"); 

      int rw = render_data->width;
      int rh = render_data->height;

      output(e).render_debug(0,0,rw,rh);
      //output(e).get().draw(0,0,rw,rh);

      TS_STOP("FlowFieldGradientSystem render");
    };

  private:

    Entity* entity;
    DepthComponent* depth_data;
    DepthProcessingComponent* depth_proc_data;
    RenderComponent* render_data;

    gpgpu::Gaussian gaussian;

    ComponentMapper<FlowFieldGradientComponent> ff_gradient_m;
    ComponentMapper<FlowFieldComponent> ff_m; 

    void update_flowfield( ofShader& shader )
    {
      FlowFieldGradientComponent* ff_data = ff_gradient_m.get( *entity ); 
      shader.setUniform1f( "max_force", ff_data->max_force );
    };

    void update_gaussian( ofShader& shader )
    {
      FlowFieldGradientComponent* ff_data = ff_gradient_m.get( *entity ); 
      shader.setUniform1f("sigma", ff_data->gaussian_sigma);
      shader.setUniform1i("kernel", ff_data->gaussian_kernel);
      shader.setUniform1f("alpha", 1.0);
    };

    gpgpu::Process& output(Entity &e)
    {
      return ff_gradient_m.get(e)->flowfield();
    };

    gpgpu::Process& input(Entity &e)
    {
      return depth_proc_data->surfaces();
    };

};

