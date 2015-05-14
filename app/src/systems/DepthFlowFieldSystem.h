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
      addComponentType<DepthComponent>();
      addComponentType<FlowFieldComponent>();
    };

    virtual void initialize() 
    {
      depth_m.init( *world );
      flowfield_m.init( *world );
    };

    virtual void added(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);
      FlowFieldComponent* ff_data = flowfield_m.get(e); 

      int w = ff_data->width;
      int h = ff_data->height;

      depth_f.init( depth_data, w, h );

      ff.init("glsl/flowfield.frag",w,h);
      debug.init("glsl/debug.frag",w,h);
    };

    virtual void processEntity(Entity &e) 
    { 
      DepthComponent* depth_data = depth_m.get(e);
      if ( ! depth_data->dirty ) return;

      FlowFieldComponent* ff_data = flowfield_m.get(e); 

      ofTexture& depth_ftex = depth_f.update( depth_data );
      ff.set( "data", depth_ftex ); 
      ff.update();

      debug.set( "data", ff.get() ); 
      debug.update();

      ff_data->field = ff.get_data();
      //ff_data->field = &(ff.get());
    };

    virtual void renderEntity(Entity &e)
    {
      FlowFieldComponent* ff_data = flowfield_m.get(e); 

      if ( !ff_data->render )
        return;

      RenderComponent* render_data = require_component<RenderComponent>("output");

      debug.get().draw( 0, 0, render_data->width, render_data->height );
    };

  private:

    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<FlowFieldComponent> flowfield_m;

    gpgpu::Process ff, debug;
    DepthFloatData depth_f;
};

