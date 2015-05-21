
/* DEPRECATED */
/* DEPRECATED */
/* DEPRECATED */
/* DEPRECATED */
/* DEPRECATED */

#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxGPGPU.h"
#include "DepthFloatData.h"

using namespace artemis;

class DepthSegmentationSystem : public ECSsystem 
{ 

  public:

    DepthSegmentationSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<DepthSegmentationComponent>();
      addComponentType<DepthComponent>();
      addComponentType<PlaneCalibComponent>();
    };

    virtual void initialize() 
    {
      segmentation_m.init( *world );
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

      ofAddListener( cml_data->cml->render_3d, this, &DepthSegmentationSystem::render_3d );

      depth_data = depth_m.get(e);
      int w = depth_data->width;
      int h = depth_data->height;

      depth_f.init( depth_data, w, h );
      //debug.init("glsl/debug.frag",w,h);
      depth_3d.init("glsl/depth_3d.frag",w,h);
      height_map.init("glsl/height_map.frag",w,h);
      depth_seg.init("glsl/depth_segmentation.frag",w,h);

      ofAddListener( depth_3d.on_update, this, &DepthSegmentationSystem::update_depth_3d );
      ofAddListener( height_map.on_update, this, &DepthSegmentationSystem::update_height_map );
      ofAddListener( depth_seg.on_update, this, &DepthSegmentationSystem::update_depth_seg );

      ofFloatPixels& output = get_output(e);
      output.allocate( w, h, channels );
      output.set(0);
      output_img.allocate(w, h, OF_IMAGE_GRAYSCALE);

      //input.allocate( w, h, channels );
      //input.set(0);
      //pix_near.allocate(w, h, channels);
      //pix_near.set(0);
      //pix_far.allocate(w, h, channels);
      //pix_far.set(0);
    }; 

    virtual void removed(Entity &e) 
    {
      ofRemoveListener( cml_data->cml->render_3d, this, &DepthSegmentationSystem::render_3d );
      ofRemoveListener( depth_3d.on_update, this, &DepthSegmentationSystem::update_depth_3d );
      ofRemoveListener( depth_seg.on_update, this, &DepthSegmentationSystem::update_height_map );
      ofRemoveListener( depth_seg.on_update, this, &DepthSegmentationSystem::update_depth_seg );

      seg_data = NULL;
      plane_calib_data = NULL;
      cml_data = NULL;
    };

    virtual void processEntity(Entity &e) 
    {
      depth_data = depth_m.get(e);
      seg_data = segmentation_m.get(e);
      plane_calib_data = plane_calib_m.get(e);

      if ( !depth_data->dirty )
        return; 

      int w = depth_data->width;
      int h = depth_data->height; 

      ofTexture& depth_ftex = depth_f.update( depth_data );

      depth_3d
        .set( "depth_map", depth_ftex )
        .update();
      height_map
        .set( "depth_3d", depth_3d.get() )
        .update();
      depth_seg
        .set( "height_map", height_map.get() )
        .update();

      //debug
        //.set( "data", depth_3d.get() )
        //.update();

      ofFloatPixels& output = get_output(e);
      //output.setFromPixels( depth_seg.get_data(), w, h, channels );

      //ofPixels* input = depth_data->depth_ofpix_grey;
      //input.setFromPixels( depth_pix_grey, w, h, channels );

      //CamaraLucidaSystem* cml_sys = require_system<CamaraLucidaSystem>();
      //float scale = 0.2;
      //ofxCv::resize( *input, output, scale, scale );
      //ofVec2f p2;
      //ofVec3f p3;
      //int ws = w*scale;
      //int hs = h*scale;
      //for ( int y = 0; y < hs; y++ )
      //for ( int x = 0; x < ws; x++ )
      //{
        //p2.set( x/scale, y/scale );
        //cml_sys->depth_to_p3( p2, p3 );
        //float d = plane_calib_data->plane.distance( p3 );
        //int i = y * ws + x;
        //output[i] = d < seg_data->threshold_far && d > seg_data->threshold_near ? 255 : 0;
      //}

      //ofxCv::copy( input, pix_near );
      //ofxCv::copy( input, pix_far );
      //ofxCv::threshold( pix_far, seg_data->threshold_far );
      //ofxCv::threshold( pix_near, seg_data->threshold_near );
      //ofxCv::add( pix_near, pix_far, output );
    }; 

    virtual void renderEntity(Entity &e)
    {
      if ( !seg_data->render ) return;

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int rw = render_data->width;
      int rh = render_data->height;

      ofFloatPixels& output = get_output(e);
      //output_img.setFromPixels( output );

      ofPushStyle();
      ofSetColor(255);

      //output_img.draw( 0, 0, rw, rh );
      //debug.get().draw( 0, 0, rw, rh );
      depth_seg.get().draw( 0, 0, rw, rh );

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
      if ( !cml_data || !depth_data ) return;

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
      if ( !seg_data ) return;
      shader.setUniform1f("threshold_near", seg_data->threshold_near);
      shader.setUniform1f("threshold_far", seg_data->threshold_far);
    };

  private:

    gpgpu::Process depth_3d, height_map, depth_seg;
    //gpgpu::Process debug;
    DepthFloatData depth_f;

    //ofPixels input; 
    //ofPixels pix_near, pix_far;
    ofImage output_img; 
    int channels;

    //ugly singleton entity stuff
    bool inited;
    DepthSegmentationComponent* seg_data;
    PlaneCalibComponent* plane_calib_data;
    CamaraLucidaComponent* cml_data;
    DepthComponent* depth_data;

    ofFloatPixels& get_output(Entity &e)
    {
      return segmentation_m.get(e)->output;
    };

    void render_3d(ofEventArgs &args)
    {
      if ( !inited ) return;
      if ( !seg_data->render ) return;

      ofVec3f n = plane_calib_data->plane.normal();
      ofVec3f ctr = plane_calib_data->triangle.centroid();

      ofPushStyle();
      ofSetColor( ofColor::cyan );
      ofSetLineWidth( 3 );
      ofLine( ctr, ctr + n * seg_data->threshold_near );
      ofSetLineWidth( 1 );
      ofLine( ctr, ctr + n * seg_data->threshold_far );
      ofPopStyle();
    };

    ComponentMapper<DepthSegmentationComponent> segmentation_m;
    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<PlaneCalibComponent> plane_calib_m;

};

