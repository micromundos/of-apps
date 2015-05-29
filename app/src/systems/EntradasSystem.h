#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxGPGPU.h"
#include "ofxTimeMeasurements.h"

using namespace artemis;

// TODO blob detection

class EntradasSystem : public ECSsystem 
{ 

  public:

    EntradasSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<EntradasComponent>();
      addComponentType<DepthComponent>();
      addComponentType<DepthProcessingComponent>();
    };

    virtual void initialize() 
    {
      entradas_m.init( *world );
      depth_m.init( *world );
      depth_processing_m.init( *world );

      scale = 0.25;
    };

    virtual void added(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);
      entradas_data = entradas_m.get(e);

      int w = depth_data->width * scale;
      int h = depth_data->height * scale;

      dilate.init("glsl/cv/dilate.frag", w, h );
      erode.init("glsl/cv/erode.frag", w, h );
      bilateral.init("glsl/cv/bilateral.frag", w, h );
      bilateral_debug.init("glsl/depth_debug.frag", w, h );

      output(e).init("glsl/height_threshold.frag", w, h ); 
      debug.init("glsl/depth_segmentation_debug.frag", w, h );

      // events

      ofAddListener( bilateral.on_update, this, &EntradasSystem::update_bilateral );

      ofAddListener( output(e).on_update, this, &EntradasSystem::update_entradas );

    }; 

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);
      entradas_data = entradas_m.get(e);

      if ( !depth_data->dirty )
        return; 

      TS_START("EntradasSystem");

      int w = depth_data->width * scale;
      int h = depth_data->height * scale;

      ofTexture _input = input(e).get_scaled(w,h);
      //ofTexture _input=height_map.get_scaled(w,h);

      bilateral
        .set( "data", _input )
        .update();
      dilate
        .set( "tex", bilateral.get() )
        //.set( "tex", _input )
        .update( 2 );
      erode
        .set( "tex", dilate.get() )
        .update( 2 );

      output(e)
        //.set( "height_map", height_map.get() )
        //.set( "height_map", bilateral.get() )
        .set( "height_map", erode.get() )
        .update();

      if ( entradas_data->render ) 
        debug
          .set( "data", output(e).get() )
          .update(); 

      if ( entradas_data->render_smoothed )
        bilateral_debug
            .set( "data", bilateral.get() )
            .update();

      TS_STOP("EntradasSystem");
    }; 

    virtual void renderEntity(Entity &e)
    { 
      if ( !entradas_data->render
          && !entradas_data->render_smoothed
        ) return;

      TS_START("EntradasSystem render");

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      if (entradas_data->render)
        debug.get().draw( 0, 0, rw, rh );

      if (entradas_data->render_smoothed)
        bilateral_debug.get().draw( 0, 0, rw, rh );

      TS_STOP("EntradasSystem render");
    };

    virtual void removed(Entity &e) 
    {
      ofRemoveListener( bilateral.on_update, this, &EntradasSystem::update_bilateral );
      ofRemoveListener( output(e).on_update, this, &EntradasSystem::update_entradas );
      entradas_data = NULL;
    };

  private:

    ComponentMapper<EntradasComponent> entradas_m;
    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<DepthProcessingComponent> depth_processing_m;

    EntradasComponent* entradas_data;

    gpgpu::Process debug;
    gpgpu::Process dilate;
    gpgpu::Process erode;
    gpgpu::Process bilateral;
    gpgpu::Process bilateral_debug;

    float scale;

    gpgpu::Process& output(Entity &e)
    {
      return entradas_m.get(e)->output;
    };

    gpgpu::Process& input(Entity &e)
    {
      return depth_processing_m.get(e)->output;
    };

    void update_bilateral( ofShader& shader )
    {
      if ( !entradas_data ) return;
      shader.setUniform1f("domain_sigma", entradas_data->bilateral_domain);
      shader.setUniform1f("range_sigma", entradas_data->bilateral_range);
      shader.setUniform1i("kernel", entradas_data->bilateral_kernel);
    };

    void update_entradas( ofShader& shader )
    {
      if ( !entradas_data ) return;
      shader.setUniform1f("threshold_near", entradas_data->threshold_near);
      shader.setUniform1f("threshold_far", entradas_data->threshold_far);
    };

};

