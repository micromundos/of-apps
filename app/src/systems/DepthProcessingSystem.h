#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxGPGPU.h"
#include "shaders/gaussian.h"
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
      entity = NULL; 
      scale = 0.5;
    };

    virtual void added(Entity &e) 
    {
      if ( entity != NULL )
      {
        ofLogWarning("DepthProcessingSystem")
          << "singleton entity already added";
        return;
      } 
      entity = &e;

      cml_data = require_component<CamaraLucidaComponent>("output");

      DepthProcessingComponent* depth_proc_data = depth_processing_m.get(e);
      DepthComponent* depth_data = depth_m.get(e);
      TableCalibComponent* table_calib_data = table_calib_m.get(e);

      int w = depth_data->width * scale;
      int h = depth_data->height * scale; 

      depth_3d
        .init("glsl/depth_3d.frag", w, h );

      //bilateral
        //.init("glsl/cv/bilateral.frag", w, h )
        //.set_debug("glsl/debug/depth_d.frag");

      //gaussian
        //.init( &gaussian_shader, w, h )
        //.set_debug("glsl/debug/depth_d.frag");

      bg_dif
        .init("glsl/background_substraction.frag", w, h ); 

      height_map(e)
        .init("glsl/height_map.frag", w, h );  

      normals(e)
        .init("glsl/normals.frag", w, h );
      //normals_bilateral
        //.init("glsl/cv/bilateral.frag", w, h )
        //.set_debug("glsl/debug/normalized_d.frag");

      plane_angles
        .init("glsl/plane_angles.frag", w, h );

      //segmentation
      surfaces(e)
        .init("glsl/segmentation.frag", w, h );

      //threshold
        //.init("glsl/height_threshold.frag", w, h );

      erode
        .add_backbuffer( "data" )
        .init("glsl/cv/erode.frag", w, h );
        //.set_debug("glsl/debug/height_d.frag");
      dilate
        .add_backbuffer( "data" )
        .init("glsl/cv/dilate.frag", w, h );

      //mask
        //.init("glsl/cv/mask.frag", w, h )
        //.set_debug("glsl/debug/height_d.frag");

      add_events(e);
    }; 

    virtual void removed(Entity &e) 
    { 
      remove_events(e);
      cml_data = NULL;
      entity = NULL;
    };

    virtual void processEntity(Entity &e) 
    {
      DepthProcessingComponent* depth_proc_data = depth_processing_m.get(e);
      DepthComponent* depth_data = depth_m.get(e);
      TableCalibComponent* table_calib_data = table_calib_m.get(e);

      if ( !depth_data->dirty )
        return; 

      TS_START("DepthProcessingSystem"); 

      ofTexture& depth_map = depth_data->f_depth_img.getTextureReference();
      //ofTexture& depth_map = depth_map_bg_dif(e);

      //ofTexture* depth_map_clean = &depth_map;

      //int open_iter = depth_proc_data->open_iter;
      //if ( open_iter > 0 )
      //{
        ///[>depth_map_clean = cv_utils::open( *depth_map_clean, open_iter, erode, dilate );
        //erode
          //.set( "data", *depth_map_clean )
          //.update( open_iter );
        //dilate
          //.set( "data", erode.get() )
          //.update( open_iter );
        //depth_map_clean = &(dilate.get());
      //}

      //int close_iter = depth_proc_data->close_iter;
      //if ( close_iter > 0 )
      //{
        ///[>depth_map_clean = cv_utils::close( *depth_map_clean, close_iter, dilate, erode );
        //dilate
          //.set( "data", *depth_map_clean )
          //.update( close_iter );
        //erode
          //.set( "data", dilate.get() )
          //.update( close_iter );
        //depth_map_clean = &(erode.get());
      //}

      //gaussian
        //.set( "data", *depth_map_clean )
        ////.set( "data", depth_3d.get() )
        //.update( 2 ); //horiz + vert

      //bilateral
        //.set( "data", depth_map )
        ////.set( "data", depth_3d.get() )
        //.update();

      depth_3d
        //.set( "depth_map", bilateral.get() )
        //.set( "depth_map", *depth_map_clean )
        //.set( "depth_map", gaussian.get() )
        .set( "depth_map", depth_map )
        .update(); 

      height_map(e)
        //.set( "mesh3d", gaussian.get() )
        //.set( "mesh3d", bilateral.get() )
        .set( "mesh3d", depth_3d.get() )
        .update(); 

      normals(e)
        //.set( "mesh3d", gaussian.get() )
        //.set( "mesh3d", bilateral.get() )
        .set( "mesh3d", depth_3d.get() )
        .update();  

      //normals_bilateral
        //.set( "data", normals(e).get() )
        //.update();

      plane_angles
        .set( "normals", normals(e).get() )
        //.set( "normals", normals_bilateral.get() )
        .update(); 

      //segmentation
      surfaces(e)
        .set( "height_map", height_map(e).get() )
        .set( "plane_angles", plane_angles.get() )
        //.set( "normals", normals_bilateral.get() )
        .update();  

      //binarize
      //threshold
        //.set( "height_map", segmentation.get() )
        //.update(); 

      //height map hi-pass
      //mask
        //.set( "data", segmentation.get() )
        //.set( "mask", dilate.get() )
        //.update();


      // update render data

      if ( depth_proc_data->render_surfaces ) 
        surfaces(e).update_debug();

      if ( depth_proc_data->render_height_map ) 
        height_map(e).update_debug();

      if ( depth_proc_data->render_normals ) 
        normals(e).update_debug();

      if ( depth_proc_data->render_smoothed )  
        gaussian.update_debug();
        //bilateral.update_debug();

      if ( depth_proc_data->render_plane_angles ) 
        plane_angles.update_debug();

      TS_STOP("DepthProcessingSystem");
    }; 

    virtual void renderEntity(Entity &e)
    {
      DepthProcessingComponent* depth_proc_data = depth_processing_m.get(e);

      if ( !depth_proc_data->render_surfaces
          && !depth_proc_data->render_normals
          && !depth_proc_data->render_height_map
          && !depth_proc_data->render_smoothed
          //&& !depth_proc_data->render_normals_smoothed
          && !depth_proc_data->render_plane_angles
       ) return;

      TS_START("DepthProcessingSystem render");

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      ofPushStyle();
      ofSetColor(255); 

      if (depth_proc_data->render_smoothed)
        gaussian.draw_debug(0,0,rw,rh);
        //bilateral.draw_debug(0,0,rw,rh);

      if (depth_proc_data->render_height_map)
        height_map(e).draw_debug(0,0,rw,rh);

      if (depth_proc_data->render_normals)
        normals(e).draw_debug(0,0,rw,rh);

      if (depth_proc_data->render_plane_angles)
        plane_angles.draw_debug(0,0,rw,rh);

      if (depth_proc_data->render_surfaces)
        surfaces(e).draw_debug(0,0,rw,rh);

      ofPopStyle();

      TS_STOP("DepthProcessingSystem render");
    };

  private:

    gpgpu::Process depth_3d;
    gpgpu::Process gaussian;
    gpgpu::Gaussian gaussian_shader;
    gpgpu::Process bilateral;
    gpgpu::Process bg_dif;
    //gpgpu::Process segmentation;
    //gpgpu::Process height_map;
    //gpgpu::Process normals;
    //gpgpu::Process normals_bilateral;
    gpgpu::Process plane_angles;
    //post-processing:
    //gpgpu::Process threshold;
    //gpgpu::Process mask;
    gpgpu::Process erode;
    gpgpu::Process dilate;

    float scale;

    Entity* entity;
    CamaraLucidaComponent* cml_data;

    ComponentMapper<DepthProcessingComponent> depth_processing_m;
    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<TableCalibComponent> table_calib_m;


    // gpu processes updates

    void update_depth_3d( ofShader& shader )
    {
      if ( cml_data == NULL || entity == NULL ) 
        return;

      DepthComponent* depth_data = depth_m.get( *entity );

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

    void update_gaussian( ofShader& shader )
    {
      DepthProcessingComponent* depth_proc_data = depth_processing_m.get( *entity );
      shader.setUniform1f("sigma", depth_proc_data->gaussian_sigma);
      shader.setUniform1i("kernel", depth_proc_data->gaussian_kernel);
    };

    void update_bilateral( ofShader& shader )
    {
      DepthProcessingComponent* depth_proc_data = depth_processing_m.get( *entity );
      shader.setUniform1f("domain_sigma", depth_proc_data->bilateral_domain);
      shader.setUniform1f("range_sigma", depth_proc_data->bilateral_range);
      shader.setUniform1i("kernel", depth_proc_data->bilateral_kernel);
    };

    void update_bg_dif( ofShader& shader )
    {
      DepthProcessingComponent* depth_proc_data = depth_processing_m.get( *entity );
      shader.setUniform1f( "threshold", depth_proc_data->threshold_background );
    };

    void update_height_map( ofShader& shader )
    {
      TableCalibComponent* table_calib_data = table_calib_m.get( *entity );
      ofxPlane& p = table_calib_data->plane;
      shader.setUniform4f( "plane", p.a, p.b, p.c, p.d );
    }; 

    void update_plane_angles( ofShader& shader )
    {
      TableCalibComponent* table_calib_data = table_calib_m.get( *entity );
      ofxPlane& p = table_calib_data->plane;
      shader.setUniform4f( "plane", p.a, p.b, p.c, p.d );
    };

    //void update_normals_bilateral( ofShader& shader )
    //{
      //DepthProcessingComponent* depth_proc_data = depth_processing_m.get( *entity );
      //shader.setUniform1f("domain_sigma", depth_proc_data->normals_bilateral_domain);
      //shader.setUniform1f("range_sigma", depth_proc_data->normals_bilateral_range);
      //shader.setUniform1i("kernel", depth_proc_data->normals_bilateral_kernel);
    //}; 

    void update_depth_segmentation( ofShader& shader )
    {
      DepthProcessingComponent* depth_proc_data = depth_processing_m.get( *entity );

      //TableCalibComponent* table_calib_data = table_calib_m.get( *entity );

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

    void render_3d(ofEventArgs &args)
    {
      if ( entity == NULL ) return;

      DepthProcessingComponent* depth_proc_data = depth_processing_m.get( *entity );
      TableCalibComponent* table_calib_data = table_calib_m.get( *entity );

      if (!depth_proc_data->render_surfaces) 
        return;

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


    // events

    void add_events(Entity &e)
    {
      if (cml_data != NULL)
        ofAddListener( cml_data->cml->render_3d, this, &DepthProcessingSystem::render_3d );

      ofAddListener( gaussian.on_update, this, &DepthProcessingSystem::update_gaussian ); 
      ofAddListener( bilateral.on_update, this, &DepthProcessingSystem::update_bilateral ); 

      ofAddListener( depth_3d.on_update, this, &DepthProcessingSystem::update_depth_3d );
      ofAddListener( bg_dif.on_update, this, &DepthProcessingSystem::update_bg_dif );
      ofAddListener( height_map(e).on_update, this, &DepthProcessingSystem::update_height_map );
      ofAddListener( plane_angles.on_update, this, &DepthProcessingSystem::update_plane_angles );
      //ofAddListener( normals_bilateral.on_update, this, &DepthProcessingSystem::update_normals_bilateral ); 
      ofAddListener( surfaces(e).on_update, this, &DepthProcessingSystem::update_depth_segmentation );
      //ofAddListener( threshold.on_update, this, &DepthProcessingSystem::update_threshold );
    };

    void remove_events(Entity &e)
    {
      if (cml_data != NULL)
        ofRemoveListener( cml_data->cml->render_3d, this, &DepthProcessingSystem::render_3d );

      ofRemoveListener( gaussian.on_update, this, &DepthProcessingSystem::update_gaussian ); 
      ofRemoveListener( bilateral.on_update, this, &DepthProcessingSystem::update_bilateral ); 

      ofRemoveListener( depth_3d.on_update, this, &DepthProcessingSystem::update_depth_3d );
      ofRemoveListener( bg_dif.on_update, this, &DepthProcessingSystem::update_bg_dif );
      ofRemoveListener( height_map(e).on_update, this, &DepthProcessingSystem::update_height_map );
      ofRemoveListener( plane_angles.on_update, this, &DepthProcessingSystem::update_plane_angles );
      //ofRemoveListener( normals_bilateral.on_update, this, &DepthProcessingSystem::update_normals_bilateral ); 
      ofRemoveListener( surfaces(e).on_update, this, &DepthProcessingSystem::update_depth_segmentation );
      //ofRemoveListener( threshold.on_update, this, &DepthProcessingSystem::update_threshold );
    };


    ofTexture& depth_map_bg_dif(Entity &e)
    {
      DepthComponent* depth_data = depth_m.get(e);
      TableCalibComponent* table_calib_data = table_calib_m.get(e);

      if (table_calib_data->background.isAllocated())
      {
        return bg_dif
          .set( "foreground", depth_data->f_depth_img.getTextureReference() )
          .set( "background", table_calib_data->background.getTextureReference() )
          .update()
          .get();
      }

      return depth_data->f_depth_img.getTextureReference();
    };


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

};

