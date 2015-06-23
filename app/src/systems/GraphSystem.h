#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

using namespace artemis;

class GraphSystem : public ECSsystem 
{ 

  public:

    GraphSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<GraphComponent>();
      addComponentType<DepthProcessingComponent>();
      addComponentType<DepthComponent>();
    };

    virtual void initialize() 
    {
      graph_m.init( *world );
      depth_processing_m.init( *world );
      depth_m.init( *world );
      entity = NULL; 
    };

    virtual void added(Entity &e) 
    {
      if ( entity != NULL )
      {
        ofLogWarning("GraphSystem")
          << "singleton entity already added";
        return;
      } 
      entity = &e;

      DepthComponent* depth_data = depth_m.get(e);

      float scale = 0.5;

      int w = depth_data->width * scale;
      int h = depth_data->height * scale; 

      nodes
        .init("glsl/graph_nodes.frag", w, h );

      //edges
        //.init("glsl/graph_edges.frag", w, h );
    }; 

    virtual void removed(Entity &e) 
    { 
      remove_events(e);
      entity = NULL;
    };

    virtual void processEntity(Entity &e) 
    {
      GraphComponent* graph_data = graph_m.get(e);
      DepthProcessingComponent* depth_proc_data = depth_processing_m.get(e);
      DepthComponent* depth_data = depth_m.get(e);

      if ( !depth_data->dirty )
        return; 

      TS_START("GraphSystem"); 

      nodes
        .set("height_map", height_map(e).get())
        .set("plane_angles", plane_angles(e).get())
        .update();

      TS_STOP("GraphSystem"); 
    }; 

    virtual void renderEntity(Entity &e)
    {
      GraphComponent* graph_data = graph_m.get(e);

      if ( ! graph_data->render )
        return;

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      ofPushStyle();

      ofSetColor( ofColor::yellow );
      nodes.get().draw( 0, 0, rw, rh );

      ofPopStyle();
    };

  private:

    Entity* entity;

    gpgpu::Process nodes;
    //gpgpu::Process edges;

    ComponentMapper<GraphComponent> graph_m;
    ComponentMapper<DepthProcessingComponent> depth_processing_m;
    ComponentMapper<DepthComponent> depth_m; 

    gpgpu::Process& height_map(Entity &e)
    {
      return depth_processing_m.get(e)->height_map();
    };

    gpgpu::Process& plane_angles(Entity &e)
    {
      return depth_processing_m.get(e)->plane_angles();
    };

    void update_nodes( ofShader& shader )
    {
      GraphComponent* graph_data = graph_m.get( *entity );
      shader.setUniform1f("threshold", graph_data->threshold_nodes);
      shader.setUniform1f("offset", graph_data->threshold_nodes_offset);
    };

    // events

    void add_events(Entity &e)
    {
      ofAddListener( nodes.on_update, this, &GraphSystem::update_nodes );
    };

    void remove_events(Entity &e)
    {
      ofRemoveListener( nodes.on_update, this, &GraphSystem::update_nodes );
    };

};

