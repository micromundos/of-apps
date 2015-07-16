#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "shaders/gaussian.h"

using namespace artemis;

class FlowFieldContainerSystem : public ECSsystem 
{ 

  public:

    FlowFieldContainerSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<FlowFieldContainerComponent>();
      addComponentType<FlowFieldComponent>();
      addComponentType<DepthComponent>();
      addComponentType<DepthProcessingComponent>();
    };

    virtual void initialize() 
    {
      flow_field_container_m.init( *world );
      flow_field_m.init( *world );
      depth_m.init( *world );
      depth_processing_m.init( *world );
      entity = NULL;
    };

    virtual void added(Entity &e) 
    {
      if ( entity != NULL )
      {
        ofLogWarning("FlowFieldContainerSystem")
          << "entity already added";
        return;
      }
      entity = &e;

      float scale = flow_field_m.get(e)->scale;
      DepthComponent* depth_data = depth_m.get(e);
      int w = depth_data->width * scale;
      int h = depth_data->height * scale;

      edges
        .add_backbuffer( "tex" )
        .init("glsl/openvision/canny.fs",w,h)
        .on( "update", this, &FlowFieldContainerSystem::update_edges )
        .set_debug("glsl/debug/height_d.frag");

      gaussian
        .init( &gaussian_shader, w, h )
        .on( "update", this, &FlowFieldContainerSystem::update_gaussian );

      bilateral
        .init("glsl/cv/bilateral.frag",w,h)
        .on( "update", this, &FlowFieldContainerSystem::update_bilateral )
        .set_debug("glsl/debug/depth_d.frag"); 

      flowfield(e)
        .init("glsl/flowfields/flowfield_container.frag",w,h);
        //.on( "update", this, &FlowFieldContainerSystem::update_flowfield );
    }; 

    virtual void removed(Entity &e) 
    {
      edges.off( "update", this, &FlowFieldContainerSystem::update_edges );
      gaussian.off( "update", this, &FlowFieldContainerSystem::update_gaussian );
      bilateral.off( "update", this, &FlowFieldContainerSystem::update_bilateral );
      //flowfield(e).off( "update", this, &FlowFieldContainerSystem::update_flowfield ); 
      entity = NULL;
    };

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);
      if ( ! depth_data->dirty ) return;

      TS_START("FlowFieldContainerSystem");

      FlowFieldContainerComponent* ff_data = flow_field_container_m.get(e); 

      ofTexture* edges_input;
      ofTexture* ff_input;

      if ( ff_data->bilateral_kernel > 0 )
      {
        bilateral
          .set( "data", height_map(e).get() )
          .update();
        edges_input = &(bilateral.get()); 
      }
      else
      {
        edges_input = &(height_map(e).get()); 
      }

      edges
        .set( "tex", *(edges_input) )
        .update();

      if ( ff_data->gaussian_kernel > 0 )
      {
        gaussian
          .set( "data", edges.get() )
          .update( 2 ); //horiz + vert
        ff_input = &(gaussian.get()); 
      }
      else
      {
        ff_input = &(edges.get()); 
      }

      flowfield(e)
        .set( "edges", *(ff_input) )
        .update();

      if ( ff_data->render )
        flowfield(e).update_debug();

      TS_STOP("FlowFieldContainerSystem");
    }; 

    virtual void renderEntity(Entity &e)
    {
      TS_START("FlowFieldContainerSystem render");

      FlowFieldContainerComponent* ff_data = flow_field_container_m.get(e); 

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      if ( ff_data->render_edges )
      {
        if ( ff_data->gaussian_kernel > 0 )
          gaussian.get().draw(0,0,rw,rh);
        else
          edges.get().draw(0,0,rw,rh);
      }

      if ( ff_data->render )
        flowfield(e).render_debug(0,0,rw,rh);

      TS_STOP("FlowFieldContainerSystem render");
    };

  private:

    Entity* entity;
    gpgpu::Process edges;
    gpgpu::Process gaussian;
    gpgpu::Gaussian gaussian_shader;
    gpgpu::Process bilateral;

    ComponentMapper<FlowFieldContainerComponent> flow_field_container_m;
    ComponentMapper<FlowFieldComponent> flow_field_m;
    ComponentMapper<DepthComponent> depth_m; 
    ComponentMapper<DepthProcessingComponent> depth_processing_m;

    void update_edges( ofShader& shader )
    {
      FlowFieldContainerComponent* ff_data = flow_field_container_m.get( *entity );
      shader.setUniform1f("threshold", ff_data->edge_detection_threshold);
    };

    void update_gaussian( ofShader& shader )
    {
      FlowFieldContainerComponent* ff_data = flow_field_container_m.get( *entity );
      shader.setUniform1f("sigma", ff_data->gaussian_sigma);
      shader.setUniform1i("kernel", ff_data->gaussian_kernel);
    };

    void update_bilateral( ofShader& shader )
    {
      FlowFieldContainerComponent* ff_data = flow_field_container_m.get( *entity );
      shader.setUniform1f("domain_sigma", ff_data->bilateral_domain);
      shader.setUniform1f("range_sigma", ff_data->bilateral_range);
      shader.setUniform1i("kernel", ff_data->bilateral_kernel);
    };

    //void update_flowfield( ofShader& shader )
    //{
      //FlowFieldComponent* ff_data = flow_field_container_m.get( *entity ); 
      //DepthProcessingComponent* depth_proc_data = depth_processing_m.get( *entity );
      //shader.setUniform1f( "xxx", xxx );
    //};

    gpgpu::Process& flowfield(Entity &e)
    {
      return flow_field_container_m.get(e)->flowfield();
    };

    gpgpu::Process& height_map(Entity &e)
    {
      return depth_processing_m.get(e)->height_map();
    };

};

