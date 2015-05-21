#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxGPGPU.h"
#include "DepthFloatData.h"
#include "ofxTimeMeasurements.h"

using namespace artemis;

class DepthProcessingSystem : public ECSsystem 
{ 

  public:

    DepthProcessingSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<DepthProcessingComponent>();
      addComponentType<DepthComponent>();
      addComponentType<PlaneCalibComponent>();
    };

    virtual void initialize() 
    {
      depth_processing_m.init( *world );
      depth_m.init( *world );
      plane_calib_m.init( *world );

      inited = false;
      channels = 1;
    };

    virtual void added(Entity &e) 
    {
      if ( inited ) return;
      inited = true; 

      cml_data = require_component<CamaraLucidaComponent>("output");

      depth_data = depth_m.get(e);
      int w = depth_data->width;
      int h = depth_data->height;

      depth_f.init( depth_data, w, h );

      debug.init("glsl/depth_process_debug.frag",w,h);
      process(e).init("glsl/depth_process.frag",w,h);
      //depth_3d.init("glsl/depth_3d.frag",w,h);
      //height_map.init("glsl/height_map.frag",w,h);
      //process(e).init("glsl/depth_segmentation.frag",w,h);

      ofAddListener( cml_data->cml->render_3d, this, &DepthProcessingSystem::render_3d );

      ofAddListener( process(e).on_update, this, &DepthProcessingSystem::update_depth_process );
      //ofAddListener( depth_3d.on_update, this, &DepthProcessingSystem::update_depth_3d );
      //ofAddListener( height_map.on_update, this, &DepthProcessingSystem::update_height_map );
      //ofAddListener( process(e).on_update, this, &DepthProcessingSystem::update_depth_seg );

      //ofFloatPixels& output = get_output(e);
      //output.allocate( w, h, channels );
      //output.set(0);
    }; 

    virtual void removed(Entity &e) 
    {
      ofRemoveListener( cml_data->cml->render_3d, this, &DepthProcessingSystem::render_3d );

      ofRemoveListener( process(e).on_update, this, &DepthProcessingSystem::update_depth_process );
      //ofRemoveListener( depth_3d.on_update, this, &DepthProcessingSystem::update_depth_3d );
      //ofRemoveListener( process(e).on_update, this, &DepthProcessingSystem::update_height_map );
      //ofRemoveListener( process(e).on_update, this, &DepthProcessingSystem::update_depth_seg );

      depth_proc_data = NULL;
      plane_calib_data = NULL;
      cml_data = NULL;
    };

    virtual void processEntity(Entity &e) 
    {
      depth_data = depth_m.get(e);
      depth_proc_data = depth_processing_m.get(e);
      plane_calib_data = plane_calib_m.get(e);

      if ( !depth_data->dirty )
        return; 

      TS_START("DepthProcessingSystem");

      int w = depth_data->width;
      int h = depth_data->height; 

      ofTexture& depth_ftex = depth_f.update( depth_data );

      process(e)
        .set( "depth_map", depth_ftex )
        .update();

      if ( depth_proc_data->render ) 
        debug
          .set( "data", process(e).get() )
          .update();

      //depth_3d
        //.set( "depth_map", depth_ftex )
        //.update();
      //height_map
        //.set( "depth_3d", depth_3d.get() )
        //.update();
      //process(e)
        //.set( "height_map", height_map.get() )
        //.update(); 

      //ofFloatPixels& output = get_output(e);
      //output.setFromPixels( depth_proc.get_data(), w, h, channels );

			TS_STOP("DepthProcessingSystem");
    }; 

    virtual void renderEntity(Entity &e)
    {
      if ( !depth_proc_data->render ) 
        return;

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      ofPushStyle();
      ofSetColor(255);

      debug.get().draw( 0, 0, rw, rh );
      //process(e).get().draw( 0, 0, rw, rh );

      ofPopStyle();
    };

    void update_depth_process( ofShader& shader )
    {
      update_depth_3d( shader );
      update_height_map( shader );
      update_depth_seg( shader );
    };

    void update_depth_3d( ofShader& shader )
    {
      if ( !cml_data || !depth_data ) 
        return;

      shader.setUniform1i("depth_flip", depth_data->flip);

      cml::DepthCamera* depth = cml_data->cml->depth_camera();
      shader.setUniform1f("width", depth->width);
      shader.setUniform1f("height", depth->height);
      shader.setUniform1f("xoff", depth->xoff);
      shader.setUniform1f("near", depth->near);
      shader.setUniform1f("far", depth->far);
      shader.setUniform1f("far_clamp", depth->far_clamp);
      shader.setUniform1f("cx", depth->cx);
      shader.setUniform1f("cy", depth->cy);
      shader.setUniform1f("fx", depth->fx);
      shader.setUniform1f("fy", depth->fy);
    };

    void update_height_map( ofShader& shader )
    {
      if ( !plane_calib_data ) return;
      ofxPlane& p = plane_calib_data->plane;
      shader.setUniform4f( "plane", p.a, p.b, p.c, p.d );
    };

    void update_depth_seg( ofShader& shader )
    {
      if ( !depth_proc_data ) return;
      shader.setUniform1f("threshold_near", depth_proc_data->threshold_near);
      shader.setUniform1f("threshold_far", depth_proc_data->threshold_far);
    };

  private:

    DepthFloatData depth_f;
    gpgpu::Process debug;
    //gpgpu::Process depth_3d,height_map;

    int channels;

    //ugly singleton entity stuff
    bool inited;
    DepthProcessingComponent* depth_proc_data;
    PlaneCalibComponent* plane_calib_data;
    CamaraLucidaComponent* cml_data;
    DepthComponent* depth_data;

    gpgpu::Process& process(Entity &e)
    {
      return depth_processing_m.get(e)->process;
    };

    //ofFloatPixels& get_output(Entity &e)
    //{
      //return depth_processing_m.get(e)->output;
    //};

    void render_3d(ofEventArgs &args)
    {
      if ( !inited ) return;
      if ( !depth_proc_data->render ) return;

      ofVec3f n = plane_calib_data->plane.normal();
      ofVec3f ctr = plane_calib_data->triangle.centroid();

      ofPushStyle();
      ofSetColor( ofColor::cyan );
      ofSetLineWidth( 3 );
      ofLine( ctr, ctr + n * depth_proc_data->threshold_near );
      ofSetLineWidth( 1 );
      ofLine( ctr, ctr + n * depth_proc_data->threshold_far );
      ofPopStyle();
    };

    ComponentMapper<DepthProcessingComponent> depth_processing_m;
    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<PlaneCalibComponent> plane_calib_m;

};

