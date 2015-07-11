#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxGPGPU.h"
#include "ofxTimeMeasurements.h"

using namespace artemis;

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
      scale = 0.5;
    };

    virtual void added(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);
      entradas_data = entradas_m.get(e);

      int w = depth_data->width * scale;
      int h = depth_data->height * scale;

      //int w = input(e).width();
      //int h = input(e).height();

      threshold
        .init("glsl/height_threshold.frag", w, h )
        .on( "update", this, &EntradasSystem::update_threshold );

      //erode
      output(e) 
        .add_backbuffer("tex")
        .init("glsl/openvision/erode.fs", w, h );

      dilate
      //output(e) 
        .add_backbuffer("tex")
        .init("glsl/openvision/dilate.fs", w, h );

      //bilateral
        //.init("glsl/cv/bilateral.frag", w, h )
        //.set_debug("glsl/debug/depth_d.frag")
        //.on( "update", this, &EntradasSystem::update_bilateral );
    }; 

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);
      entradas_data = entradas_m.get(e);

      if ( !depth_data->dirty )
        return; 

      TS_START("EntradasSystem");

      //int w = depth_data->width * scale;
      //int h = depth_data->height * scale;
      //ofTexture _input = input(e).get_scaled(w,h);
      ofTexture& _input = input(e).get();

      //bilateral
        //.set( "data", _input )
        //.update();

      threshold
        .set( "height_map", _input )
        .update();

      int close_iter = entradas_data->close_iter;
      if ( close_iter < 1 )
      {
        ofLogWarning("EntradasSystem")
          << " close_iter is " << close_iter
          << " and 1 is minimum required";
        close_iter = 1;
      }

      //cv_utils::close( threshold.get(), close_iter, dilate, output(e) );
      dilate
        .set( "tex", threshold.get() )
        .update( close_iter );
      //erode
      output(e) 
        .set( "tex", dilate.get() )
        .update( close_iter ); 

      //if ( entradas_data->render ) 
        //output(e).update_debug(); 

      //if ( entradas_data->render_smoothed )
        //bilateral.update_debug();

      TS_STOP("EntradasSystem");
    }; 

    virtual void renderEntity(Entity &e)
    { 
      if ( !entradas_data->render
          //&& !entradas_data->render_smoothed
        ) return;

      TS_START("EntradasSystem render");

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      if (entradas_data->render)
        output(e).get().draw(0,0,rw,rh);
        //output(e).draw_debug(0,0,rw,rh);

      //if (entradas_data->render_smoothed)
        //bilateral.draw_debug(0,0,rw,rh);

      TS_STOP("EntradasSystem render");
    };

    virtual void removed(Entity &e) 
    {
      //bilateral.off( "update", this, &EntradasSystem::update_bilateral );
      threshold.off( "update", this, &EntradasSystem::update_threshold );
      entradas_data = NULL;
    };

  private:

    float scale;

    ComponentMapper<EntradasComponent> entradas_m;
    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<DepthProcessingComponent> depth_processing_m;

    EntradasComponent* entradas_data;

    gpgpu::Process threshold;
    gpgpu::Process erode;
    gpgpu::Process dilate;
    //gpgpu::Process bilateral;

    gpgpu::Process& output(Entity &e)
    {
      return entradas_m.get(e)->output();
    };

    gpgpu::Process& input(Entity &e)
    {
      return depth_processing_m.get(e)->surfaces();
    };

    //void update_bilateral( ofShader& shader )
    //{
      //if ( !entradas_data ) return;
      //shader.setUniform1f("domain_sigma", entradas_data->bilateral_domain);
      //shader.setUniform1f("range_sigma", entradas_data->bilateral_range);
      //shader.setUniform1i("kernel", entradas_data->bilateral_kernel);
    //};

    void update_threshold( ofShader& shader )
    {
      if ( !entradas_data ) return;
      shader.setUniform1i("binary", 1);
      shader.setUniform1f("threshold_near", entradas_data->threshold_near);
      shader.setUniform1f("threshold_far", entradas_data->threshold_far);
    };

};

