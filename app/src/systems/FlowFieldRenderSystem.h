#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

using namespace artemis;

class FlowFieldRenderSystem : public ECSsystem 
{ 

  public:

    FlowFieldRenderSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<FlowFieldRenderComponent>();
      addComponentType<FlowFieldComponent>();
    };

    virtual void initialize() 
    {
      ffr_m.init( *world );
      ff_m.init( *world );
      inited = false;
    };

    virtual void removed(Entity &e) 
    {
    };

    virtual void added(Entity &e) 
    {
      if ( inited )
      {
        ofLogWarning("FlowFieldRenderSystem")
          << "entity already added";
        return;
      }
      inited = true;

      if ( ! shader.load( "glsl/flowfields/flowfield_render.vert", "glsl/flowfields/flowfield_render.frag" ) )
      {
        ofLogError("FlowFieldRenderSystem") 
          << "shader failed to load";
        return;
      }

      mesh.setMode(OF_PRIMITIVE_LINES);

      FlowFieldComponent* ff_data = ff_m.get(e);
      gpgpu::Process& flowfield = ff_data->flowfield();
      int w = flowfield.width();
      int h = flowfield.height();

      float step = 2;

      ofColor tail( ofColor::crimson );
      ofColor head( ofColor::crimson, 0 );

      for ( int y = 0; y < h; y += step )
      {
        for ( int x = 0; x < w; x += step )
        {
          //tail
          mesh.addVertex( ofVec2f(x,y) );
          mesh.addTexCoord( ofVec2f(x,y) );
          mesh.addColor( tail );
          //head
          mesh.addVertex( ofVec2f(x,y) );
          mesh.addTexCoord( ofVec2f(x,y) );
          mesh.addColor( head );
        }
      }
    }; 

    virtual void processEntity(Entity &e) 
    {
    }; 

    virtual void renderEntity(Entity &e)
    {
      //DepthComponent* depth_data = component<DepthComponent>("input");
      //if ( ! depth_data->dirty ) return;

      FlowFieldRenderComponent* ffr_data = ffr_m.get(e);
      if ( ! ffr_data->render ) return;

      FlowFieldComponent* ff_data = ff_m.get(e);
      gpgpu::Process& flowfield = ff_data->flowfield();
      int w = flowfield.width();
      int h = flowfield.height();

      RenderComponent* render_data = component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      ofPushStyle();
      ofSetLineWidth( ffr_data->line_width ); 
      ofSetColor(255);
      shader.begin();
      shader.setUniformTexture( "flowfield", flowfield.get(), 0 );
      shader.setUniform1f( "force_min", ffr_data->force_min );
      shader.setUniform1f( "force_max", ffr_data->force_max );
      shader.setUniform2f( "size", w, h );
      shader.setUniform2f( "render_size", rw, rh );
      mesh.draw();
      shader.end();
      ofPopStyle();
    };

  private:

    ofVboMesh mesh;
    ofShader shader;

    bool inited;

    ComponentMapper<FlowFieldRenderComponent> ffr_m;
    ComponentMapper<FlowFieldComponent> ff_m; 

};

