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
      addComponentType<DepthComponent>();
      addComponentType<DepthProcessingComponent>();
    };

    virtual void initialize() 
    {
      ff_gradient_m.init( *world );
      ff_m.init( *world );
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

      float scale = ff_m.get(e)->scale;
      DepthComponent* depth_data = depth_m.get(e);
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
    };

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);
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

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      output(e).render_debug(0,0,rw,rh);
      //output(e).get().draw(0,0,rw,rh);

      TS_STOP("FlowFieldGradientSystem render");
    };

  private:

    Entity* entity;
    gpgpu::Gaussian gaussian;

    ComponentMapper<FlowFieldGradientComponent> ff_gradient_m;
    ComponentMapper<FlowFieldComponent> ff_m;
    ComponentMapper<DepthComponent> depth_m; 
    ComponentMapper<DepthProcessingComponent> depth_processing_m;

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
      return depth_processing_m.get(e)->surfaces();
    };

};

