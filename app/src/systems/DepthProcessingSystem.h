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
      scale = 0.5;
    };

    virtual void added(Entity &e) 
    {
      if ( inited ) return;
      inited = true; 

      cml_data = require_component<CamaraLucidaComponent>("output");

      depth_data = depth_m.get(e);

      int dw = depth_data->width;
      int dh = depth_data->height;
      int w = dw * scale;
      int h = dh * scale;

      depth_f.init( depth_data, dw, dh );
      depth_3d.init("glsl/depth_3d.frag",dw,dh);

      height_map.init("glsl/height_map.frag",w,h);

      //depth_bilateral.init("glsl/bilateral.frag",dw,dh);
      //depth_bilateral_debug.init("glsl/depth_debug.frag",dw,dh);

      normals.init("glsl/normals.frag",w,h);
      normals_debug.init("glsl/debug.frag",w,h);

      normals_bilateral.init("glsl/bilateral.frag",w,h);
      normals_bilateral_debug.init("glsl/debug.frag",w,h);

      output(e).init("glsl/depth_segmentation.frag",w,h);
      output_debug.init("glsl/depth_segmentation_debug.frag",w,h);

      ofAddListener( cml_data->cml->render_3d, this, &DepthProcessingSystem::render_3d );

      ofAddListener( depth_3d.on_update, this, &DepthProcessingSystem::update_depth_3d );
      ofAddListener( height_map.on_update, this, &DepthProcessingSystem::update_height_map );
      ofAddListener( output(e).on_update, this, &DepthProcessingSystem::update_depth_seg );
    }; 

    virtual void removed(Entity &e) 
    {
      ofRemoveListener( cml_data->cml->render_3d, this, &DepthProcessingSystem::render_3d );

      ofRemoveListener( depth_3d.on_update, this, &DepthProcessingSystem::update_depth_3d );
      ofRemoveListener( output(e).on_update, this, &DepthProcessingSystem::update_height_map );
      ofRemoveListener( output(e).on_update, this, &DepthProcessingSystem::update_depth_seg );

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

      ofTexture* depth_ftex = NULL;

      TS_START("DepthProcessingSystem update depth data");
      depth_ftex = &(depth_f.update( depth_data ));
      TS_STOP("DepthProcessingSystem update depth data");

      TS_START("DepthProcessingSystem");

      //depth_bilateral
        //.set( "data", *depth_ftex )
        //.update();

      depth_3d
        .set( "depth_map", *depth_ftex )
        //.set( "depth_map", depth_bilateral.get() )
        .update();

      ofTexture depth_3d_scaled = depth_3d.get_scaled( scale );

      normals
        .set( "depth_3d", depth_3d_scaled )
        .update(); 

      normals_bilateral
        .set( "data", normals.get() )
        .update();

      height_map
        .set( "depth_3d", depth_3d_scaled )
        .update();

      output(e)
        .set( "height_map", height_map.get() )
        .set( "normals", normals_bilateral.get() )
        .update(); 

      if ( depth_proc_data->render ) 
        output_debug
          .set( "data", output(e).get() )
          .update();

      if ( depth_proc_data->render_normals ) 
        normals_debug
          .set( "data", normals.get() )
          .update();

      if ( depth_proc_data->render_smooth ) 
        //depth_bilateral_debug
          //.set( "data", depth_bilateral.get() )
          //.update();
        normals_bilateral_debug
          .set( "data", normals_bilateral.get() )
          .update();

      TS_STOP("DepthProcessingSystem");
    }; 

    virtual void renderEntity(Entity &e)
    {
      if ( !depth_proc_data->render
          && !depth_proc_data->render_normals
          && !depth_proc_data->render_smooth
       ) return;

      TS_START("DepthProcessingSystem render");

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      ofPushStyle();
      ofSetColor(255);

      if (depth_proc_data->render)
        output_debug.get().draw( 0, 0, rw, rh );

      if (depth_proc_data->render_normals)
        normals_debug.get().draw( 0, 0, rw, rh );

      if (depth_proc_data->render_smooth)
        //depth_bilateral_debug.get().draw( 0, 0, rw, rh );
        normals_bilateral_debug.get().draw( 0, 0, rw, rh );

      ofPopStyle();

      TS_STOP("DepthProcessingSystem render");
    };

    void update_depth_3d( ofShader& shader )
    {
      if ( !cml_data || !depth_data ) 
        return;

      shader.setUniform1i("depth_flip", depth_data->flip);

      cml::DepthCamera* depthcam = cml_data->cml->depth_camera();

      shader.setUniform1f("width", depthcam->width);
      shader.setUniform1f("height", depthcam->height);
      shader.setUniform1f("xoff", depthcam->xoff);
      shader.setUniform1f("near", depthcam->near);
      shader.setUniform1f("far", depthcam->far);
      shader.setUniform1f("far_clamp", depthcam->far_clamp);
      shader.setUniform1f("cx", depthcam->cx);
      shader.setUniform1f("cy", depthcam->cy);
      shader.setUniform1f("fx", depthcam->fx);
      shader.setUniform1f("fy", depthcam->fy);
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
      if ( !plane_calib_data ) return;

      ofxPlane& p = plane_calib_data->plane;
      shader.setUniform4f( "plane", p.a, p.b, p.c, p.d );
      shader.setUniform1f("threshold_plane", depth_proc_data->threshold_plane);

      shader.setUniform1f("threshold_near", depth_proc_data->threshold_near);
      shader.setUniform1f("threshold_far", depth_proc_data->threshold_far);
    };

  private:

    gpgpu::Process output_debug;

    DepthFloatData depth_f;
    gpgpu::Process depth_3d;

    gpgpu::Process normals;
    gpgpu::Process normals_debug;

    gpgpu::Process normals_bilateral;
    gpgpu::Process normals_bilateral_debug;

    //gpgpu::Process depth_bilateral;
    //gpgpu::Process depth_bilateral_debug;

    gpgpu::Process height_map;

    float scale;
    int channels;

    //ugly singleton entity stuff
    bool inited;
    DepthProcessingComponent* depth_proc_data;
    PlaneCalibComponent* plane_calib_data;
    CamaraLucidaComponent* cml_data;
    DepthComponent* depth_data;

    gpgpu::Process& output(Entity &e)
    {
      return depth_processing_m.get(e)->output;
    };

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

