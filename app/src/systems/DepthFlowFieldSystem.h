#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGPGPU.h"

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
      channels = 1;
    };

    virtual void added(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);
      FlowFieldComponent* ff_data = flowfield_m.get(e); 

      int w = ff_data->width;
      int h = ff_data->height;

      f_depth_pix_mm.allocate( depth_data->width, depth_data->height, channels );
      f_depth_pix_mm.set(0);

      f_depth_pix_resized.allocate( w, h, channels );
      f_depth_pix_resized.set(0);

      f_depth_tex.allocate( w, h, GL_LUMINANCE32F_ARB );

      ff.init("glsl/flowfield.frag",w,h);
      ff_debug.init("glsl/flowfield_debug.frag",w,h);
    };

    virtual void processEntity(Entity &e) 
    { 
      DepthComponent* depth_data = depth_m.get(e);
      if ( ! depth_data->dirty ) return;

      FlowFieldComponent* ff_data = flowfield_m.get(e); 

      f_depth_pix_mm.setFromPixels( depth_data->f_depth_ofpix_mm->getPixels(), depth_data->width, depth_data->height, channels );

      float xscale = (float)ff_data->width / depth_data->width; 
      float yscale = (float)ff_data->height / depth_data->height; 
      ofxCv::resize( f_depth_pix_mm, f_depth_pix_resized, xscale, yscale );

      f_depth_tex.loadData( f_depth_pix_resized );

      ff.set( "data", f_depth_tex ); 
      ff.update();

      ff_debug.set( "data", ff.get() ); 
      ff_debug.update();

      ff_data->field = ff.get_data();
      //ff_data->field = &(ff.get());
    };

    virtual void renderEntity(Entity &e)
    {
      FlowFieldComponent* ff_data = flowfield_m.get(e); 

      if ( !ff_data->render )
        return;

      RenderComponent* render_data = require_component<RenderComponent>("output");

      ff_debug.get().draw( 0, 0, 
          render_data->width, render_data->height );
    };

  private:

    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<FlowFieldComponent> flowfield_m;

    gpgpu::Process ff, ff_debug;
    ofTexture f_depth_tex;
    ofFloatPixels f_depth_pix_mm;
    ofFloatPixels f_depth_pix_resized;
    int channels;
};

