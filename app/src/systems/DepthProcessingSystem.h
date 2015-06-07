#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxGPGPU.h"
#include "ofxTimeMeasurements.h"

using namespace artemis;

class DepthProcessingSystem : public ECSsystem 
{ 

  public:

    DepthProcessingSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<DepthProcessingComponent>();
      addComponentType<DepthComponent>();
      addComponentType<TableCalibComponent>();
    };

    virtual void initialize() 
    {
      depth_processing_m.init( *world );
      depth_m.init( *world );
      table_calib_m.init( *world );

      inited = false;
      scale = 0.5;
    };

    virtual void added(Entity &e) 
    {
      if ( inited ) return;
      inited = true; 

      cml_data = require_component<CamaraLucidaComponent>("output");

      depth_proc_data = depth_processing_m.get(e);
      depth_data = depth_m.get(e);
      table_calib_data = table_calib_m.get(e);

      int depth_w = depth_data->width;
      int depth_h = depth_data->height;
      int w = depth_w * scale;
      int h = depth_h * scale;

      //processes at depth size

      depth_map_bilateral
        .init("glsl/cv/bilateral.frag", depth_w, depth_h )
        .set_debug("glsl/depth_debug.frag");

      depth_3d.init("glsl/depth_3d.frag", depth_w, depth_h );

      background_substraction.init("glsl/cv/background_substraction.frag", depth_w, depth_h ); 

      //down sampled processes

      height_map(e).init("glsl/height_map.frag",w,h);  

      normals(e).init("glsl/normals.frag",w,h);
      //normals_bilateral
        //.init("glsl/cv/bilateral.frag", w, h )
        //.set_debug("glsl/debug_normalized.frag");

      plane_angles.init("glsl/plane_angles.frag", w, h );

      segmentation.init("glsl/depth_segmentation.frag", w, h );

      //post-processing
      //threshold.init("glsl/height_threshold.frag", w, h );
      erode.init("glsl/cv/erode.frag", w, h );
      dilate.init("glsl/cv/dilate.frag", w, h );
      surfaces(e)
        .init("glsl/cv/erode.frag", w, h )
        .set_debug("glsl/height_debug.frag");
      //mask
        //.init("glsl/cv/mask.frag", w, h )
        //.set_debug("glsl/height_debug.frag");

      add_events(e);
    }; 

    virtual void removed(Entity &e) 
    { 
      remove_events(e);
      depth_proc_data = NULL;
      table_calib_data = NULL;
      cml_data = NULL;
    };

    virtual void processEntity(Entity &e) 
    {
      depth_proc_data = depth_processing_m.get(e);
      depth_data = depth_m.get(e);
      table_calib_data = table_calib_m.get(e);

      if ( !depth_data->dirty )
        return; 

      TS_START("DepthProcessingSystem"); 

      ofTexture* depth_map;
      if ( table_calib_data->background.isAllocated() )
      {
        depth_map = &(background_substraction
          .set( "foreground", depth_data->f_depth_img.getTextureReference() )
          .set( "background", table_calib_data->background.getTextureReference() )
          .update()
          .get());
      }
      else
      {
        depth_map = &(depth_data->f_depth_img.getTextureReference());
      }

      depth_map_bilateral
        .set( "data", *depth_map )
        .update();

      depth_3d
        .set("depth_map", depth_map_bilateral.get())
        //.set( "depth_map", *depth_map )
        .update();

      ofTexture depth_3d_scaled = depth_3d.get_scaled( scale ); 

      height_map(e)
        .set( "mesh_3d", depth_3d_scaled )
        .update(); 

      normals(e)
        .set( "mesh_3d", depth_3d_scaled )
        .update();  

      //normals_bilateral
        //.set( "data", normals(e).get() )
        //.update();

      plane_angles
        .set( "normals", normals(e).get() )
        //.set( "normals", normals_bilateral.get() )
        .update(); 

      segmentation
        .set( "height_map", height_map(e).get() )
        .set( "plane_angles", plane_angles.get() )
        //.set( "normals", normals_bilateral.get() )
        .update();  

      //binarize
      //threshold
        //.set( "height_map", segmentation.get() )
        //.update();
      //open -> close = rm noise -> rm holes
      erode
        .set( "tex", segmentation.get() )
        //.set( "tex", threshold.get() )
        .update( 1 );
      dilate
        .set( "tex", erode.get() )
        .update( 1 );
      surfaces(e)
        .set( "tex", dilate.get() )
        .update( 4 );
      //height map hi-pass
      //mask
        //.set( "data", segmentation.get() )
        //.set( "mask", dilate.get() )
        //.update();


      // update render data

      if ( depth_proc_data->render_surfaces ) 
        surfaces(e).update_debug();

      if ( depth_proc_data->render_normals ) 
        normals(e).update_debug();

      if ( depth_proc_data->render_depth_map_smoothed )  
        depth_map_bilateral.update_debug();

      //if ( depth_proc_data->render_normals_smoothed )  
        //normals_bilateral.update_debug();

      if ( depth_proc_data->render_plane_angles ) 
        plane_angles.update_debug();

      TS_STOP("DepthProcessingSystem");
    }; 

    virtual void renderEntity(Entity &e)
    {
      if ( !depth_proc_data->render_surfaces
          && !depth_proc_data->render_normals
          && !depth_proc_data->render_depth_map_smoothed
          //&& !depth_proc_data->render_normals_smoothed
          && !depth_proc_data->render_plane_angles
       ) return;

      TS_START("DepthProcessingSystem render");

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      ofPushStyle();
      ofSetColor(255);

      if (depth_proc_data->render_surfaces)
        //surfaces(e).get().draw(0,0,rw,rh);
        surfaces(e).draw_debug(0,0,rw,rh);

      if (depth_proc_data->render_normals)
        normals(e).draw_debug(0,0,rw,rh);

      if (depth_proc_data->render_plane_angles)
        plane_angles.draw_debug(0,0,rw,rh); 

      if (depth_proc_data->render_depth_map_smoothed)
        depth_map_bilateral.draw_debug(0,0,rw,rh);

      //if (depth_proc_data->render_normals_smoothed)
        //normals_bilateral.draw_debug(0,0,rw,rh); 

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

    void update_depth_map_bilateral( ofShader& shader )
    {
      if ( !depth_proc_data ) return;
      shader.setUniform1f("domain_sigma", depth_proc_data->depth_map_bilateral_domain);
      shader.setUniform1f("range_sigma", depth_proc_data->depth_map_bilateral_range);
      shader.setUniform1i("kernel", depth_proc_data->depth_map_bilateral_kernel);
    };

    void update_background_substraction( ofShader& shader )
    {
      if ( !depth_proc_data ) return;
      shader.setUniform1f( "threshold", depth_proc_data->threshold_background );
    };

    void update_height_map( ofShader& shader )
    {
      if ( !table_calib_data ) return;
      ofxPlane& p = table_calib_data->plane;
      shader.setUniform4f( "plane", p.a, p.b, p.c, p.d );
    }; 

    void update_plane_angles( ofShader& shader )
    {
      if ( !table_calib_data ) return;
      ofxPlane& p = table_calib_data->plane;
      shader.setUniform4f( "plane", p.a, p.b, p.c, p.d );
    };

    //void update_normals_bilateral( ofShader& shader )
    //{
      //if ( !depth_proc_data ) return;
      //shader.setUniform1f("domain_sigma", depth_proc_data->normals_bilateral_domain);
      //shader.setUniform1f("range_sigma", depth_proc_data->normals_bilateral_range);
      //shader.setUniform1i("kernel", depth_proc_data->normals_bilateral_kernel);
    //}; 

    void update_depth_segmentation( ofShader& shader )
    {
      if ( !depth_proc_data ) return;
      if ( !table_calib_data ) return;

      //ofxPlane& p = table_calib_data->plane;
      //shader.setUniform4f( "plane", p.a, p.b, p.c, p.d );
      shader.setUniform1f("threshold_plane", depth_proc_data->threshold_plane);

      shader.setUniform1f("threshold_near", depth_proc_data->threshold_near);
      shader.setUniform1f("threshold_far", depth_proc_data->threshold_far);
    };

    void update_threshold( ofShader& shader )
    {
      shader.setUniform1i("binary", 1);
      shader.setUniform1f("threshold_near", 0.);
      shader.setUniform1f("threshold_far", 5000.);
    };

    void add_events(Entity &e)
    {
      if (cml_data != NULL)
        ofAddListener( cml_data->cml->render_3d, this, &DepthProcessingSystem::render_3d );

      ofAddListener( depth_map_bilateral.on_update, this, &DepthProcessingSystem::update_depth_map_bilateral ); 
      ofAddListener( depth_3d.on_update, this, &DepthProcessingSystem::update_depth_3d );
      ofAddListener( background_substraction.on_update, this, &DepthProcessingSystem::update_background_substraction );
      ofAddListener( height_map(e).on_update, this, &DepthProcessingSystem::update_height_map );
      ofAddListener( plane_angles.on_update, this, &DepthProcessingSystem::update_plane_angles );
      //ofAddListener( normals_bilateral.on_update, this, &DepthProcessingSystem::update_normals_bilateral ); 
      ofAddListener( segmentation.on_update, this, &DepthProcessingSystem::update_depth_segmentation );
      //ofAddListener( threshold.on_update, this, &DepthProcessingSystem::update_threshold );
    };

    void remove_events(Entity &e)
    {
      if (cml_data != NULL)
        ofRemoveListener( cml_data->cml->render_3d, this, &DepthProcessingSystem::render_3d );

      ofRemoveListener( depth_map_bilateral.on_update, this, &DepthProcessingSystem::update_depth_map_bilateral ); 
      ofRemoveListener( depth_3d.on_update, this, &DepthProcessingSystem::update_depth_3d );
      ofRemoveListener( background_substraction.on_update, this, &DepthProcessingSystem::update_background_substraction );
      ofRemoveListener( height_map(e).on_update, this, &DepthProcessingSystem::update_height_map );
      ofRemoveListener( plane_angles.on_update, this, &DepthProcessingSystem::update_plane_angles );
      //ofRemoveListener( normals_bilateral.on_update, this, &DepthProcessingSystem::update_normals_bilateral ); 
      ofRemoveListener( segmentation.on_update, this, &DepthProcessingSystem::update_depth_segmentation );
      //ofRemoveListener( threshold.on_update, this, &DepthProcessingSystem::update_threshold );
    };

  private:

    gpgpu::Process depth_3d;
    gpgpu::Process depth_map_bilateral;
    gpgpu::Process background_substraction;
    //downsampled
    gpgpu::Process segmentation;
    //gpgpu::Process height_map;
    //gpgpu::Process normals;
    //gpgpu::Process normals_bilateral;
    gpgpu::Process plane_angles;
    //post-processing
    //gpgpu::Process threshold;
    //gpgpu::Process mask;
    gpgpu::Process erode;
    gpgpu::Process dilate;

    float scale;

    //ugly singleton entity stuff
    bool inited;
    DepthProcessingComponent* depth_proc_data;
    TableCalibComponent* table_calib_data;
    CamaraLucidaComponent* cml_data;
    DepthComponent* depth_data;

    //output shortcuts

    gpgpu::Process& surfaces(Entity &e)
    {
      return depth_processing_m.get(e)->surfaces();
    };

    gpgpu::Process& height_map(Entity &e)
    {
      return depth_processing_m.get(e)->height_map();
    };

    gpgpu::Process& normals(Entity &e)
    {
      return depth_processing_m.get(e)->normals();
    };

    void render_3d(ofEventArgs &args)
    {
      if ( !inited ) return;
      if (!depth_proc_data->render_surfaces) return;

      ofVec3f n = table_calib_data->plane.normal();
      ofVec3f ctr = table_calib_data->triangle.centroid();

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
    ComponentMapper<TableCalibComponent> table_calib_m;

};

