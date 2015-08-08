/*
 * flow field container process: 
 *  input: clean height map
 *  bilateral: smooth surface keeping edges
 *  canny: edge detection
 *  gaussian: smooth edges
 */

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
        .init( "glsl/openvision/canny.fs", w, h )
        .on( "update", this, &FlowFieldContainerSystem::update_edges )
        .set_debug("glsl/debug/height_d.frag");

      gaussian
        .init( w, h )
        .on( "update", this, &FlowFieldContainerSystem::update_gaussian );

      bilateral
        .init("glsl/cv/bilateral.frag",w,h)
        .on( "update", this, &FlowFieldContainerSystem::update_bilateral )
        .set_debug("glsl/debug/depth_d.frag"); 

      output(e)
        .init("glsl/flowfields/flowfield_container.frag",w,h);
        //.on( "update", this, &FlowFieldContainerSystem::update_flowfield );
    }; 

    virtual void removed(Entity &e) 
    {
      edges.off( "update", this, &FlowFieldContainerSystem::update_edges );
      gaussian.off( "update", this, &FlowFieldContainerSystem::update_gaussian );
      bilateral.off( "update", this, &FlowFieldContainerSystem::update_bilateral );
      //output(e).off( "update", this, &FlowFieldContainerSystem::update_flowfield ); 
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
          .set( "data", input(e).get() )
          .update();
        edges_input = &(bilateral.get()); 
      }
      else
      {
        edges_input = &(input(e).get()); 
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

      output(e)
        .set( "edges", *(ff_input) )
        .update()
        .update_debug( ff_data->render );

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
        output(e).render_debug(0,0,rw,rh);

      TS_STOP("FlowFieldContainerSystem render");
    };

  private:

    Entity* entity;
    gpgpu::Process edges;
    gpgpu::Gaussian gaussian;
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
      shader.setUniform1f("alpha", 1.0);
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

    gpgpu::Process& output(Entity &e)
    {
      return flow_field_container_m.get(e)->flowfield();
    };

    gpgpu::Process& input(Entity &e)
    {
      return depth_processing_m.get(e)->surfaces();
    };

};

