#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGPGPU.h"
#include "DepthFloatData.h"

using namespace artemis;

class DepthFlowFieldSystem : public ECSsystem 
{ 

  public:

    DepthFlowFieldSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<DepthProcessingComponent>();
      addComponentType<DepthComponent>();
      addComponentType<FlowFieldComponent>();
      addComponentType<FlowfieldDebugComponent>();
    };

    virtual void initialize() 
    {
      depth_m.init( *world );
      flowfield_m.init( *world );
      depth_processing_m.init( *world );
      flowfield_debug_m.init( *world );
    };

    virtual void added(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);

      //depth_f.init( depth_data, w, h );

      gpgpu::Process& input = get_input(e);
      int w = input.width()/2;
      int h = input.height()/2;
      
      ff_resized_input.allocate(w,h,GL_RGBA32F_ARB);

      process(e).init("glsl/flowfield.frag",w,h,true);
      debug.init("glsl/debug.frag",w,h);
      
      ofAddListener(process(e).on_update, this, &DepthFlowFieldSystem::onShaderUpdate);

    };
    
    void  onShaderUpdate(ofShader &shader)
    {
      shader.setUniform1f("force_amplifier",ff_force_amplifier);
      shader.setUniform1f("ramp_margin",ff_ramp_margin);

    }

    virtual void processEntity(Entity &e)
    { 
      DepthComponent* depth_data = depth_m.get(e);
      if ( ! depth_data->dirty ) return;

      DepthProcessingComponent* depth_proc_data = depth_processing_m.get(e); 
      FlowfieldDebugComponent*  flowfield_debug = flowfield_debug_m.get(e);
      FlowFieldComponent*       flowfield_data = flowfield_m.get(e);
      //ofTexture& depth_ftex = depth_f.update( depth_data );

      ff_force_amplifier = flowfield_data->force_amplifier;
      ff_ramp_margin = flowfield_data->ramp_margin;
      
      ff_resized_input.begin();
      ofClear(0,255);
      ofSetColor(255);
      depth_proc_data->process.get().draw(0,0,320,240);
      ff_resized_input.end();
      
      process(e)
        //.set( "data", depth_ftex )
        .set( "data", ff_resized_input.getTextureReference())
        .update();

      debug
        .set( "data", process(e).get() )
        .update();
      
      flowfield_debug->flowfield_data = &process(e).get();
      
      
    };

    virtual void renderEntity(Entity &e)
    {
      FlowFieldComponent* ff_data = flowfield_m.get(e); 
      DepthProcessingComponent* depth_proc_data = depth_processing_m.get(e);

      if ( !ff_data->render )
        return;

      RenderComponent* render_data = require_component<RenderComponent>("output");

      debug.get().draw( 0, 0, render_data->width, render_data->height );
      
      /*
      depth_proc_data->process.get().draw(0,0,ff_resized_input.getWidth(),ff_resized_input.getHeight());

      ff_resized_input.draw(ff_resized_input.getWidth(),0);
      */
      //process(e).get().draw( 0, 0, render_data->width, render_data->height );
      //get_input(e).get().draw( 0, 0, render_data->width, render_data->height );
    };

  private:

    ComponentMapper<DepthProcessingComponent> depth_processing_m;
    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<FlowFieldComponent> flowfield_m;
    ComponentMapper<FlowfieldDebugComponent>  flowfield_debug_m;

    gpgpu::Process debug;
    float          ff_force_amplifier;
    float          ff_ramp_margin;
    ofFbo          ff_resized_input;
    //DepthFloatData depth_f;

    gpgpu::Process& process(Entity &e)
    {
      return flowfield_m.get(e)->process;
    };

    gpgpu::Process& get_input(Entity &e)
    {
      return depth_processing_m.get(e)->process;
    };
};

