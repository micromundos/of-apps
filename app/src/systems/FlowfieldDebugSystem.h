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

      depth_data = NULL;
      render_data = NULL;
      inited = false;

      dot_step = 1;
      mesh.setMode(OF_PRIMITIVE_POINTS);

      shader.setGeometryInputType(GL_POINTS);
      shader.setGeometryOutputType(GL_LINE_STRIP);
      shader.setGeometryOutputCount(6);
      shader.load("glsl/flowfield_debug/debug.vert","glsl/flowfield_debug/debug.frag","glsl/flowfield_debug/debug.geom");
    };

    virtual void removed(Entity &e) 
    {
      depth_data = NULL;
      render_data = NULL;
    };

    virtual void added(Entity &e) 
    {
      depth_data = component<DepthComponent>("input");
      render_data = component<RenderComponent>("output");

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

      //ofFbo::Settings s;
      //s.internalformat = GL_RGBA;
      //s.width = render_data->width;
      //s.height = render_data->height;
      //fbo.allocate( s );
      //fbo.begin();
      //ofClear(0,255);
      //fbo.end();

      inited = true;
    }; 

    virtual void processEntity(Entity &e) 
    {};

    virtual void renderEntity(Entity &e)
    {
      //if ( ! depth_data->dirty ) return;

      FlowfieldDebugComponent* ffd_data = ffd_m.get(e);
      FlowFieldComponent* ff_data = ff_m.get(e);

      if ( ! ffd_data->render ) return;

      int rw = render_data->width;
      int rh = render_data->height;

      gpgpu::Process& flowfield = ff_data->flowfield();
      int w = flowfield.width();
      int h = flowfield.height();

      ofVec2f io_scale = ofVec2f( 
        (float)rw / w, 
        (float)rh / h 
      );

      //fbo.bind();

      ofPushStyle();
      ofSetColor(255);
      ofSetLineWidth(1.0);
      glPointSize(0.0);
      shader.begin();
      shader.setUniformTexture("data", flowfield.get(), 0);
      shader.setUniform1f("multx", io_scale.x);
      shader.setUniform1f("multy", io_scale.y);
      shader.setUniform1f("line_long", dot_step);
      shader.setUniform1f("step",dot_step);
      mesh.draw();
      shader.end();
      ofPopStyle();

      //fbo.unbind();

      //fbo.draw(0,0);
    };

  private:

    DepthComponent* depth_data;
    RenderComponent* render_data;

    ComponentMapper<FlowfieldDebugComponent> ffd_m;
    ComponentMapper<FlowFieldComponent> ff_m;

    ofVboMesh mesh;
    ofShader shader;
    //ofFbo fbo;

    int       dot_step;
    bool      inited;

};

