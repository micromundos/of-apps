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
      inited = false;
      debug_mesh.setMode(OF_PRIMITIVE_POINTS);
      output_shader.setGeometryInputType(GL_POINTS);
      output_shader.setGeometryOutputType(GL_LINE_STRIP);
      output_shader.setGeometryOutputCount(6);
      output_shader.load("glsl/flowfield_debug/debug.vert","glsl/flowfield_debug/debug.frag","glsl/flowfield_debug/debug.geom");
      
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
      //ofLogNotice("FlowfieldDebugSystem") << "process entity " << e.getId();
      //flowfield_debug_m.get(e)->data;
      FlowfieldDebugComponent* debug_data = flowfield_debug_m.get(e);
      
      if(!debug_data)
        return;
      
      if(!inited && debug_data != NULL && debug_data->flowfield_data != NULL)
      {
        //  para adaptarse a la resoluciòn que venga
        inited = true;
        dot_step = 2;
        
        for(int y=0;y<debug_data->flowfield_data->getHeight();y+=dot_step)
        {
          for(int x=0;x<debug_data->flowfield_data->getWidth();x+=dot_step)
          {
            debug_mesh.addVertex(ofVec2f(x,y));
            debug_mesh.addColor(ofColor(255,0,0));
          }
        }
      }
      
      
      
    };

    virtual void renderEntity(Entity &e)
    {
      
      FlowfieldDebugComponent* debug_data = flowfield_debug_m.get(e);
      RenderComponent* render_data = require_component<RenderComponent>("output");
      
      if ( !debug_data->render )
        return;
      
      if(debug_data != NULL && debug_data->flowfield_data != NULL){
        
        
        ofVec2f input_output_scale = ofVec2f(render_data->width/debug_data->flowfield_data->getWidth(),render_data->height/debug_data->flowfield_data->getHeight());
        
        ofSetColor(255);
        ofPushStyle();
        output_shader.begin();
        output_shader.setUniformTexture("data", *debug_data->flowfield_data, 0);
        output_shader.setUniform1f("multx", input_output_scale.x);
        output_shader.setUniform1f("multy", input_output_scale.y);
        output_shader.setUniform1f("line_long", dot_step);
        output_shader.setUniform1f("step",dot_step);
        debug_mesh.draw();
        output_shader.end();
        ofPopStyle();
        
        
      }
      
      
      
    };

  private:

    ComponentMapper<FlowfieldDebugComponent> flowfield_debug_m;
    ofMesh    debug_mesh;
    ofShader  output_shader;
    int       dot_step;
    bool      inited;

};

