#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

using namespace artemis;

class DepthSmoothingSystem : public ECSsystem 
{ 

  public:

    DepthSmoothingSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<DepthSmoothingComponent>();
      addComponentType<DepthSegmentationComponent>();
      addComponentType<DepthComponent>();
    };

    virtual void initialize() 
    {
      smooth_m.init( *world );
      segmentation_m.init( *world );
      depth_m.init( *world ); 
      inited = false;
      channels = 1;
    };

    virtual void added(Entity &e) 
    {
      if (inited) return;
      inited = true;

      ofPixels& input = get_input(e);
      ofPixels& output = get_output(e);

      DepthSmoothingComponent* smooth_data = smooth_m.get(e);

      int w = (float)input.getWidth() * smooth_data->img_scale;
      int h = (float)input.getHeight() * smooth_data->img_scale;

      output.allocate( w, h, channels );
      output.set(0);
      output_img.allocate(w, h, OF_IMAGE_GRAYSCALE);
    }; 

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);
      DepthSmoothingComponent* smooth_data = smooth_m.get(e);

      if ( !depth_data->dirty )
        return;

      ofPixels& input = get_input(e);
      ofPixels& output = get_output(e);

      ofxCv::resize( input, output, smooth_data->img_scale, smooth_data->img_scale );

      ofxCv::blur( output, smooth_data->blur_size );
      ofxCv::dilate( output, 1 );
      ofxCv::erode( output, 1 );
      ofxCv::dilate( output, 1 );
    }; 

    virtual void renderEntity(Entity &e)
    {
      DepthSmoothingComponent* smooth_data = smooth_m.get(e);

      if ( !smooth_data->render )
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

    ofImage output_img; 
    int channels;

    ofPixels& get_input(Entity &e)
    {
      return segmentation_m.get(e)->output;
    };

    ofPixels& get_output(Entity &e)
    {
      return smooth_m.get(e)->output;
    };

    bool inited;

    ComponentMapper<DepthSmoothingComponent> smooth_m;
    ComponentMapper<DepthSegmentationComponent> segmentation_m;
    ComponentMapper<DepthComponent> depth_m;

};

