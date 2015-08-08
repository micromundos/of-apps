#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxGPGPU.h"
#include "shaders/gaussian.h"

using namespace artemis;

class DepthProcessingSystem : public ECSsystem 
{ 

  public:

    DepthProcessingSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<DepthProcessingComponent>();
      addComponentType<DepthComponent>();
      addComponentType<TableCalibComponent>();
      //addComponentType<DepthHoleFillerComponent>();
    };

    virtual void initialize() 
    {
      depth_processing_m.init( *world );
      depth_m.init( *world );
      table_calib_m.init( *world );
      //depth_hole_filler_m.init( *world );
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
        .init("glsl/depth_3d.frag", 
            depth_data->width, 
            depth_data->height )
        .on( "update", this, &DepthProcessingSystem::update_depth_3d );

      bg_dif
        .init("glsl/background_substraction.frag", w, h )
        .on( "update", this, &DepthProcessingSystem::update_bg_dif );

      bg_dif_expand
        .add_backbuffer( "backbuffer" )
        .init("glsl/background_substraction_expand.frag", w, h )
        .on( "update", this, &DepthProcessingSystem::update_bg_dif_expand );

      //bilateral
        //.init("glsl/cv/bilateral.frag", w, h )
        //.set_debug("glsl/debug/depth_d.frag")
        //.on( "update", this, &DepthProcessingSystem::update_bilateral ); 

      //gaussian
        //.init( w, h )
        //.set_debug("glsl/debug/depth_d.frag")
        //.on( "update", this, &DepthProcessingSystem::update_gaussian );  

      height_map(e)
        .init("glsl/height_map.frag", w, h )
        .on( "update", this, &DepthProcessingSystem::update_height_map );

      normals(e)
        .init("glsl/normals.frag", w, h );

      //normals_bilateral
        //.init("glsl/cv/bilateral.frag", w, h )
        //.set_debug("glsl/debug/normalized_d.frag")
        //.on( "update", this, &DepthProcessingSystem::update_normals_bilateral ); 

      //table_angles(e)
        //.init("glsl/plane_angles.frag", w, h )
        //.on( "update", this, &DepthProcessingSystem::update_table_angles );

      surfaces(e) //segmentation
        .init("glsl/segmentation.frag", w, h )
        .on( "update", this, &DepthProcessingSystem::update_depth_segmentation );

      erode
        .add_backbuffer( "tex" )
        .init("glsl/openvision/erode.fs", w, h );
        //.set_debug("glsl/debug/height_d.frag");

      dilate
        .add_backbuffer( "tex" )
        .init("glsl/openvision/dilate.fs", w, h );

      ofAddListener( cml_data->cml->render_3d, this, &DepthProcessingSystem::render_3d );
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
      //DepthHoleFillerComponent* depth_hole_filler_data = depth_hole_filler_m.get(e);

      if ( !depth_data->dirty )
        return; 

      TS_START("DepthProcessingSystem"); 

      ofTexture* depth_map;

      // depth hole filler
      //if ( depth_hole_filler_data->enabled && depth_hole_filler_data->output.isAllocated() )
      //{
        //depth_map = &( get_depth_map( depth_3d, e, depth_hole_filler_data->output ).getTextureReference() );
      //}

      // depth map
      //else if ( depth_data->f_depth_img.isAllocated() )
      //{
        depth_map = &( get_depth_map( depth_3d, e, depth_data->f_depth_img ).getTextureReference() );
      //}

      depth_3d
        .set( "depth_map", *depth_map )
        .update(); 

      height_map(e)
        .set( "mesh3d", depth_3d.get() )
        .update()
        .update_debug( depth_proc_data->render_height_map );

      normals(e)
        .set( "mesh3d", depth_3d.get() )
        .update()
        .update_debug( depth_proc_data->render_normals );


      // process surfaces = clean height map

      ofTexture* _height_map_surfaces;
      _height_map_surfaces = &(height_map(e).get());

      //table background diff
      if ( table_calib_data->background_height_map.isAllocated() && depth_proc_data->bg_dif )
      {
        //lazy init
        //if (!background_height_map.isAllocated())
        //{
          //background_height_map = scale table_calib_data->background_height_map to fit bg_dif process;
        //}

        ofTexture& _bg_dif = bg_dif
          .set( "foreground", *_height_map_surfaces )
          .set( "background", table_calib_data->background_height_map.getTextureReference() )
          .update()
          .get();

        if ( depth_proc_data->bg_dif_expand_kernel > 0 )
        {
          _height_map_surfaces = &(bg_dif_expand
            .set( "backbuffer", _bg_dif )
            .set( "foreground", *_height_map_surfaces )
            .set( "background", table_calib_data->background_height_map.getTextureReference() )
            .update()
            .get());
        }
        else
        {
          _height_map_surfaces = &(_bg_dif);
        }
      }

      int open_iter = depth_proc_data->open_iter;
      if ( open_iter > 0 )
      {
        erode
          .set( "tex", *_height_map_surfaces )
          .update( open_iter );
        dilate
          .set( "tex", erode.get() )
          .update( open_iter );
        _height_map_surfaces = &(dilate.get());
      }

      int close_iter = depth_proc_data->close_iter;
      if ( close_iter > 0 )
      {
        dilate
          .set( "tex", *_height_map_surfaces )
          .update( close_iter );
        erode
          .set( "tex", dilate.get() )
          .update( close_iter );
        _height_map_surfaces = &(erode.get());
      }

      surfaces(e) //segmentation
        .set( "height_map", *_height_map_surfaces )
        .update()
        .update_debug( depth_proc_data->render_surfaces );

      // update render data

      //gaussian.update_debug( depth_proc_data->render_smoothed );
      //bilateral.update_debug( depth_proc_data->render_smoothed );
      //table_angles(e).update_debug( depth_proc_data->render_table_angles );

      TS_STOP("DepthProcessingSystem");
    }; 

    virtual void renderEntity(Entity &e)
    {
      TS_START("DepthProcessingSystem render");

      DepthProcessingComponent* depth_proc_data = depth_processing_m.get(e);

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      ofPushStyle();
      ofSetColor(255); 

      //if (depth_proc_data->render_smoothed)
        //gaussian.render_debug(0,0,rw,rh);
        //bilateral.render_debug(0,0,rw,rh);

      if (depth_proc_data->render_height_map)
        height_map(e).render_debug(0,0,rw,rh);

      if (depth_proc_data->render_normals)
        normals(e).render_debug(0,0,rw,rh);

      //if (depth_proc_data->render_table_angles)
        //table_angles(e).render_debug(0,0,rw,rh);

      if (depth_proc_data->render_surfaces)
        surfaces(e).render_debug(0,0,rw,rh);

      ofPopStyle();

      TS_STOP("DepthProcessingSystem render");
    };

  private:

    gpgpu::Process depth_3d;
    gpgpu::Process bg_dif;
    gpgpu::Process bg_dif_expand;
    gpgpu::Process erode;
    gpgpu::Process dilate;
    //gpgpu::Gaussian gaussian;
    //gpgpu::Process bilateral;
    //gpgpu::Process normals_bilateral;

    float scale;
    ofFloatImage fimg_scaled;
    ofFloatPixels fpix_scaled;

    //ofFloatImage background_height_map;

    Entity* entity;
    CamaraLucidaComponent* cml_data;

    ComponentMapper<DepthProcessingComponent> depth_processing_m;
    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<TableCalibComponent> table_calib_m;
    //ComponentMapper<DepthHoleFillerComponent> depth_hole_filler_m;


    ofFloatImage& get_depth_map( gpgpu::Process& proc, Entity& e, ofFloatImage& src )
    {
      DepthComponent* depth_data = depth_m.get(e);
      int dw = depth_data->width;
      int dh = depth_data->height;

      //scaled
      int src_w = src.getWidth();
      int src_h = src.getHeight();
      int dst_w = proc.width();
      int dst_h = proc.height();
      if ( src_w == dst_w && src_h == dst_h ) 
        return src;
      float xscale = (float)dst_w / src_w;
      float yscale = (float)dst_h / src_h;
      ofxCv::resize( src.getPixelsRef(), fpix_scaled, xscale, yscale, cv::INTER_LINEAR ); //INTER_LINEAR, INTER_NEAREST, INTER_AREA, INTER_CUBIC, INTER_LANCZOS4
      fimg_scaled.setFromPixels( fpix_scaled );
      return fimg_scaled;

      //ensure depth size 
      //if ( src.getWidth() == dw && src.getHeight() == dh ) return src;
      //_depth_map.clone( src );
      //_depth_map.update();
      //_depth_map.resize( dw, dh );
      //return _depth_map;

      //scaled
      //int dst_w = dw * scale;
      //int dst_h = dh * scale;
      //if ( src.getWidth() == dst_w && src.getHeight() == dst_h ) return src;
      //_depth_map.clone( src );
      //_depth_map.update();
      //_depth_map.resize( dst_w, dst_h );
      //return _depth_map; 
    };


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

    //void update_gaussian( ofShader& shader )
    //{
      //DepthProcessingComponent* depth_proc_data = depth_processing_m.get( *entity );
      //shader.setUniform1f("sigma", depth_proc_data->gaussian_sigma);
      //shader.setUniform1i("kernel", depth_proc_data->gaussian_kernel);
    //};

    //void update_bilateral( ofShader& shader )
    //{
      //DepthProcessingComponent* depth_proc_data = depth_processing_m.get( *entity );
      //shader.setUniform1f("domain_sigma", depth_proc_data->bilateral_domain);
      //shader.setUniform1f("range_sigma", depth_proc_data->bilateral_range);
      //shader.setUniform1i("kernel", depth_proc_data->bilateral_kernel);
    //};

    void update_bg_dif( ofShader& shader )
    {
      DepthProcessingComponent* depth_proc_data = depth_processing_m.get( *entity );
      shader.setUniform1f( "threshold", depth_proc_data->bg_dif_threshold );
    };

    void update_bg_dif_expand( ofShader& shader )
    {
      DepthProcessingComponent* depth_proc_data = depth_processing_m.get( *entity );
      shader.setUniform1i( "kernel", (int)(depth_proc_data->bg_dif_expand_kernel) );
    };

    void update_height_map( ofShader& shader )
    {
      TableCalibComponent* table_calib_data = table_calib_m.get( *entity );
      ofxPlane& p = table_calib_data->plane;
      shader.setUniform4f( "plane", p.a, p.b, p.c, p.d );
    }; 

    //void update_table_angles( ofShader& shader )
    //{
      //TableCalibComponent* table_calib_data = table_calib_m.get( *entity );
      //ofxPlane& p = table_calib_data->plane;
      //shader.setUniform4f( "plane", p.a, p.b, p.c, p.d );
    //};

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

      //shader.setUniform1f("threshold_table_angle", depth_proc_data->threshold_table_angle);
      shader.setUniform1f("threshold_table_near", depth_proc_data->threshold_table_near);
      shader.setUniform1f("threshold_table_far", depth_proc_data->threshold_table_far);
    };

    //void update_threshold( ofShader& shader )
    //{
      //shader.setUniform1i("binary", 1);
      //shader.setUniform1f("threshold_near", 0.);
      //shader.setUniform1f("threshold_far", 5000.);
    //};

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
      ofLine( ctr, ctr + n * depth_proc_data->threshold_table_near );
      ofSetLineWidth( 1 );
      ofLine( ctr, ctr + n * depth_proc_data->threshold_table_far );
      ofPopStyle();
    };


    // events

    void remove_events(Entity &e)
    {
      if (cml_data != NULL)
        ofRemoveListener( cml_data->cml->render_3d, this, &DepthProcessingSystem::render_3d );

      bg_dif.off( "update", this, &DepthProcessingSystem::update_bg_dif );
      bg_dif_expand.off( "update", this, &DepthProcessingSystem::update_bg_dif_expand );
      depth_3d.off( "update", this, &DepthProcessingSystem::update_depth_3d );
      height_map(e).off( "update", this, &DepthProcessingSystem::update_height_map );
      surfaces(e).off( "update", this, &DepthProcessingSystem::update_depth_segmentation );
      //bilateral.off( "update", this, &DepthProcessingSystem::update_bilateral ); 
      //gaussian.off( "update", this, &DepthProcessingSystem::update_gaussian ); 
      //normals_bilateral.off( "update", this, &DepthProcessingSystem::update_normals_bilateral ); 
      //table_angles.off( "update", this, &DepthProcessingSystem::update_table_angles );
    };


    //shortcuts

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

    //gpgpu::Process& table_angles(Entity &e)
    //{
      //return depth_processing_m.get(e)->table_angles();
    //};

};

