#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

using namespace artemis;

class FlowfieldDebugSystem : public ECSsystem 
{ 

  public:

    FlowfieldDebugSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<FlowfieldDebugComponent>();
    };

    virtual void initialize() 
    {
      flowfield_debug_m.init( *world );
      debug_inited = false;
      
      field_mesh.setMode(OF_PRIMITIVE_POINTS);
      field_shader.setGeometryInputType(GL_POINTS);
      field_shader.setGeometryOutputType(GL_LINE_STRIP);
      field_shader.setGeometryOutputCount(6);
      field_shader.load("glsl/flowfield_debug.vert","glsl/flowfield_debug.frag","glsl/flowfield_debug.geom");

    
      
    };

    //virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    //{
      //EntityProcessingSystem::processEntities(bag);
      //int len = bag.getCount();
      //for ( int i = 0; i < len; i++ )
        //processEntity( *bag.get(i) );
    //};

    //virtual void renderEntities( ImmutableBag<Entity*>& bag ) 
    //{
      //EntityProcessingSystem::renderEntities(bag);
    //};

    virtual void added(Entity &e) 
    {
      //flowfield_debug_m.get(e)->data;
    }; 

    virtual void processEntity(Entity &e) 
    {
      
      FlowfieldDebugComponent*  ff_debug_data = flowfield_debug_m.get(e);
      
      if(!ff_debug_data)
        return;
      
      if(!debug_inited)
      {
        if(ff_debug_data->flow_field.getWidth() > 0)
        {
          debug_inited = true;
          debug_shader.init("glsl/flowfield_debug_.frag",ff_debug_data->flow_field.getWidth(),ff_debug_data->flow_field.getHeight());
          
          RenderComponent* render_data = require_component<RenderComponent>("output");
          
          
          
          for(int y=0;y<ff_debug_data->flow_field.getHeight();y+=1)
          {
            for(int x=0;x<ff_debug_data->flow_field.getWidth();x+=1)
            {
              field_mesh.addVertex(ofVec2f(x,y));
              field_mesh.addColor(ofColor(255,0,0));
            }
          }
          
        }
      }else{
        
        
        debug_shader.set( "data", ff_debug_data->flow_field);
        debug_shader.update();

        
      }
    }; 

    virtual void renderEntity(Entity &e)
    {
      
      FlowfieldDebugComponent* debug_data = flowfield_debug_m.get(e);
      
      if ( !debug_data->render)
        return;
      
      if(debug_inited){
        RenderComponent* render_data = require_component<RenderComponent>("output");
        
        ofVec2f input_output_scale = ofVec2f(render_data->width/debug_data->flow_field.getWidth(),render_data->height/debug_data->flow_field.getHeight());
        
        ofSetColor(255);
        ofPushStyle();
        //ofSetLineWidth(10.0);
        field_shader.begin();
        field_shader.setUniformTexture("data", debug_data->flow_field, 0);
        field_shader.setUniform1f("multx", input_output_scale.x);
        field_shader.setUniform1f("multy", input_output_scale.y);
        field_shader.setUniform1f("line_long", input_output_scale.x);
        field_shader.setUniform1f("step",1.0);
        field_mesh.draw();
        field_shader.end();
        ofPopStyle();
      }
    
    };

  private:

    ComponentMapper<FlowfieldDebugComponent> flowfield_debug_m;
    gpgpu::Process  debug_shader;
    bool            debug_inited;
    ofShader        field_shader;
    ofMesh          field_mesh;
  
};

