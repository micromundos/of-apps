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
    };

    virtual void initialize() 
    {
      segmentation_m.init( *world );
      depth_m.init( *world );
      inited = false;
      channels = 1;
    };

    virtual void added(Entity &e) 
    {
      if (inited) return;
      inited = true; 

      ofPixels& output = get_output(e);

      DepthComponent* depth_data = depth_m.get(e);
      int w = depth_data->width;
      int h = depth_data->height;

      output.allocate( w, h, channels );
      output.set(0);
      output_img.allocate(w, h, OF_IMAGE_GRAYSCALE);

      input.allocate( w, h, channels );
      input.set(0);

      pix_near.allocate(w, h, channels);
      pix_near.set(0);

      pix_far.allocate(w, h, channels);
      pix_far.set(0);
    }; 

    virtual void processEntity(Entity &e) 
    {
      DepthSegmentationComponent* seg_data = segmentation_m.get(e);
      DepthComponent* depth_data = depth_m.get(e);

      if ( !depth_data->dirty )
        return;

      ofPixels& output = get_output(e);

      int w = depth_data->width;
      int h = depth_data->height;

      uint8_t *depth_pix_grey = depth_data->depth_pix_grey;

      input.setFromPixels( depth_pix_grey, w, h, channels );

      ofxCv::copy( input, pix_near );
      ofxCv::copy( input, pix_far );
      ofxCv::threshold( pix_far, seg_data->threshold_far );
      ofxCv::threshold( pix_near, seg_data->threshold_near );
      ofxCv::add( pix_near, pix_far, output );
    }; 

    virtual void renderEntity(Entity &e)
    {
      DepthSegmentationComponent* seg_data = segmentation_m.get(e);

      if ( !seg_data->render )
        return;

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

    ofPixels input; 
    ofPixels pix_near; 
    ofPixels pix_far;
    ofImage output_img; 
    int channels;

    ofPixels& get_output(Entity &e)
    {
      return segmentation_m.get(e)->output;
    };

    bool inited;

    ComponentMapper<DepthSegmentationComponent> segmentation_m;
    ComponentMapper<DepthComponent> depth_m;

};

