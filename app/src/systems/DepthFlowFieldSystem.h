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
      int w = input.width();
      int h = input.height();

      process(e).init("glsl/flowfield.frag",w,h,true);
      debug.init("glsl/debug.frag",w,h);
    };

    virtual void processEntity(Entity &e) 
    { 
      DepthComponent* depth_data = depth_m.get(e);
      if ( ! depth_data->dirty ) return;

      DepthProcessingComponent* depth_proc_data = depth_processing_m.get(e); 
      FlowfieldDebugComponent*  flowfield_debug = flowfield_debug_m.get(e);
      //ofTexture& depth_ftex = depth_f.update( depth_data );

      process(e)
        //.set( "data", depth_ftex )
        .set( "data", depth_proc_data->process.get() )
        .update();

      debug
        .set( "data", process(e).get() )
        .update();
      
      flowfield_debug->flowfield_data = &debug.get();
      
      
    };

    virtual void renderEntity(Entity &e)
    {
      FlowFieldComponent* ff_data = flowfield_m.get(e); 

      if ( !ff_data->render )
        return;

      RenderComponent* render_data = require_component<RenderComponent>("output");

      debug.get().draw( 0, 0, render_data->width, render_data->height );
      
      
      //process(e).get().draw( 0, 0, render_data->width, render_data->height );
      //get_input(e).get().draw( 0, 0, render_data->width, render_data->height );
    };

  private:

    ComponentMapper<DepthProcessingComponent> depth_processing_m;
    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<FlowFieldComponent> flowfield_m;
    ComponentMapper<FlowfieldDebugComponent>  flowfield_debug_m;

    gpgpu::Process debug;
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

