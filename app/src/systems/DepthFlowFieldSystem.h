#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGPGPU.h"
#include "DepthFloatData.h"
#include "ofxTimeMeasurements.h"

using namespace artemis;

class DepthFlowFieldSystem : public ECSsystem 
{ 

  public:

    DepthFlowFieldSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<DepthProcessingComponent>();
      addComponentType<DepthComponent>();
      addComponentType<FlowFieldComponent>();
    };

    virtual void initialize() 
    {
      depth_m.init( *world );
      flowfield_m.init( *world );
      depth_processing_m.init( *world );
    };

    virtual void added(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);

      //depth_f.init( depth_data, w, h );

      gpgpu::Process& _input = input(e);
      int w = _input.width();
      int h = _input.height();

      output(e).init("glsl/flowfield.frag",w,h);
      debug.init("glsl/debug.frag",w,h);
    };

    virtual void processEntity(Entity &e) 
    { 
      DepthComponent* depth_data = depth_m.get(e);
      if ( ! depth_data->dirty ) return;

      TS_START("DepthFlowFieldSystem");

      //DepthProcessingComponent* depth_proc_data = depth_processing_m.get(e); 
      //ofTexture& depth_ftex = depth_f.update( depth_data );

      output(e)
        //.set( "data", depth_ftex )
        .set( "data", input(e).get() )
        .update();

      debug
        .set( "data", output(e).get() )
        .update();

      TS_STOP("DepthFlowFieldSystem");
    };

    virtual void renderEntity(Entity &e)
    {
      FlowFieldComponent* ff_data = flowfield_m.get(e); 

      if ( !ff_data->render )
        return;

      RenderComponent* render_data = require_component<RenderComponent>("output");

      debug.get().draw( 0, 0, render_data->width, render_data->height );
      //output(e).get().draw( 0, 0, render_data->width, render_data->height );
      //input(e).get().draw( 0, 0, render_data->width, render_data->height );
    };

  private:

    ComponentMapper<DepthProcessingComponent> depth_processing_m;
    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<FlowFieldComponent> flowfield_m;

    gpgpu::Process debug;
    //DepthFloatData depth_f;

    gpgpu::Process& output(Entity &e)
    {
      return flowfield_m.get(e)->output;
    };

    gpgpu::Process& input(Entity &e)
    {
      return depth_processing_m.get(e)->output;
    };
};

