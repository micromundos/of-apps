#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

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

      ofAddListener( require_component<CamaraLucidaComponent>("output")->cml->render_3d, this, &DepthSegmentationSystem::render_3d );

      ofPixels& output = get_output(e);

      DepthComponent* depth_data = depth_m.get(e);
      int w = depth_data->width;
      int h = depth_data->height;

      output.allocate( w, h, channels );
      output.set(0);
      output_img.allocate(w, h, OF_IMAGE_GRAYSCALE);

      //input.allocate( w, h, channels );
      //input.set(0);

      pix_near.allocate(w, h, channels);
      pix_near.set(0);

      pix_far.allocate(w, h, channels);
      pix_far.set(0);
    }; 

    virtual void removed(Entity &e) 
    {
      ofRemoveListener( require_component<CamaraLucidaComponent>("output")->cml->render_3d, this, &DepthSegmentationSystem::render_3d );
    };

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);

      seg_data = segmentation_m.get(e);
      plane_calib_data = plane_calib_m.get(e);

      if ( !depth_data->dirty )
        return; 

      CamaraLucidaSystem* cml_sys = require_system<CamaraLucidaSystem>();

      //keep goin
      ofPixels& output = get_output(e);

      int w = depth_data->width;
      int h = depth_data->height;

      ofPixels* input = depth_data->depth_ofpix_grey;
      //input.setFromPixels( depth_pix_grey, w, h, channels );

      //TODO usar ofxGPGPU

      float scale = 0.2;
      ofxCv::resize( *input, output, scale, scale );

      ofVec2f p2;
      ofVec3f p3;

      int ws = w*scale;
      int hs = h*scale;
      for ( int y = 0; y < hs; y++ )
      for ( int x = 0; x < ws; x++ )
      {
        p2.set( x/scale, y/scale );
        cml_sys->depth_to_p3( p2, p3 );
        float d = plane_calib_data->plane.distance( p3 );
        int i = y * ws + x;
        output[i] = d < seg_data->threshold_far && d > seg_data->threshold_near ? 255 : 0;
      }

      //ofxCv::copy( input, pix_near );
      //ofxCv::copy( input, pix_far );
      //ofxCv::threshold( pix_far, seg_data->threshold_far );
      //ofxCv::threshold( pix_near, seg_data->threshold_near );
      //ofxCv::add( pix_near, pix_far, output );
    }; 

    virtual void renderEntity(Entity &e)
    {
      //seg_data = segmentation_m.get(e);
      //plane_calib_data = plane_calib_m.get(e);

      if ( !seg_data->render ) return;

      RenderComponent* render_data = component<RenderComponent>("output");
      int w = render_data->width;
      int h = render_data->height;

      ofPixels& output = get_output(e);
      output_img.setFromPixels( output );

      ofPushStyle();
      ofSetColor(255);
      output_img.draw( 0, 0, w, h );
      ofPopStyle(); 
    };

  private:

    //ofPixels input; 
    ofPixels pix_near; 
    ofPixels pix_far;
    ofImage output_img; 
    int channels;

    //ugly singleton entity stuff
    bool inited;
    DepthSegmentationComponent* seg_data;
    PlaneCalibComponent* plane_calib_data;

    ofPixels& get_output(Entity &e)
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

