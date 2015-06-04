#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGPGPU.h"
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

      gpgpu::Process& _surfaces = surfaces(e);
      int w = _surfaces.width();
      int h = _surfaces.height();

      flowfield(e).init("glsl/flowfield.frag",w,h);
    };

    virtual void processEntity(Entity &e) 
    { 
      DepthComponent* depth_data = depth_m.get(e);
      FlowFieldComponent* ff_data = flowfield_m.get(e); 

      if ( ! depth_data->dirty ) return;

      TS_START("DepthFlowFieldSystem");

      flowfield(e)
        .set( "data", surfaces(e).get() )
        .update();

      if ( ff_data->render )
        flowfield(e).update_debug();

      TS_STOP("DepthFlowFieldSystem");
    };

    virtual void renderEntity(Entity &e)
    {
      FlowFieldComponent* ff_data = flowfield_m.get(e); 

      if ( !ff_data->render )
        return;

      TS_START("DepthFlowFieldSystem render");

      RenderComponent* render_data = require_component<RenderComponent>("output");

      flowfield(e).draw_debug( 0, 0, render_data->width, render_data->height );
      //flowfield(e).get().draw( 0, 0, render_data->width, render_data->height );
      //surfaces(e).get().draw( 0, 0, render_data->width, render_data->height );

      TS_STOP("DepthFlowFieldSystem render");
    };

  private:

    ComponentMapper<DepthProcessingComponent> depth_processing_m;
    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<FlowFieldComponent> flowfield_m;

    gpgpu::Process& flowfield(Entity &e)
    {
      return flowfield_m.get(e)->flowfield();
    };

    gpgpu::Process& surfaces(Entity &e)
    {
      return depth_processing_m.get(e)->surfaces();
    };

    gpgpu::Process& height_map(Entity &e)
    {
      return depth_processing_m.get(e)->height_map();
    };
};

