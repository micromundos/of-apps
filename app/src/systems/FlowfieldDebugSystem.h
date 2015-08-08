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
      addComponentType<FlowFieldComponent>();
    };

    virtual void initialize() 
    {
      ffd_m.init( *world );
      ff_m.init( *world );

      inited = false;

      mesh.setMode(OF_PRIMITIVE_POINTS);

      shader.setGeometryInputType(GL_POINTS);
      shader.setGeometryOutputType(GL_LINE_STRIP);
      shader.setGeometryOutputCount(6);
      shader.load("glsl/flowfield_debug/debug.vert","glsl/flowfield_debug/debug.frag","glsl/flowfield_debug/debug.geom");
    };

    virtual void added(Entity &e) 
    {
      inited = true;
      dot_step = 2;

      FlowFieldComponent* ff_data = ff_m.get(e);
      gpgpu::Process& flowfield = ff_data->flowfield();

      int w = flowfield.width();
      int h = flowfield.height();

      for ( int y = 0; y < h; y += dot_step )
      {
        for ( int x = 0; x < w; x += dot_step )
        {
          mesh.addVertex(ofVec2f(x,y));
          mesh.addColor(ofColor(255,0,0));
        }
      }
    }; 

    virtual void processEntity(Entity &e) 
    {};

    virtual void renderEntity(Entity &e)
    {

      FlowfieldDebugComponent* ffd_data = ffd_m.get(e);
      FlowFieldComponent* ff_data = ff_m.get(e);

      if ( !ffd_data->render )
        return;

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      gpgpu::Process& flowfield = ff_data->flowfield();
      int w = flowfield.width();
      int h = flowfield.height();

      ofVec2f io_scale = ofVec2f( 
        (float)rw / w, 
        (float)rh / h 
      );

      ofSetColor(255);
      ofPushStyle();
      shader.begin();
      shader.setUniformTexture("data", flowfield.get(), 0);
      shader.setUniform1f("multx", io_scale.x);
      shader.setUniform1f("multy", io_scale.y);
      shader.setUniform1f("line_long", dot_step);
      shader.setUniform1f("step",dot_step);
      mesh.draw();
      shader.end();
      ofPopStyle();
    };

  private:

    ComponentMapper<FlowfieldDebugComponent> ffd_m;
    ComponentMapper<FlowFieldComponent> ff_m;

    ofMesh    mesh;
    ofShader  shader;
    int       dot_step;
    bool      inited;

};

