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
      blur_size = 5;
      img_scale = 0.15;
    };

    //virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    //{
      //EntityProcessingSystem::processEntities(bag);
      //int len = bag.getCount();
      //for ( int i = 0; i < len; i++ )
        //processEntity( *bag.get(i) );
    //};

    //virtual void renderEntities( ImmutableBag<Entity*>& bag ) 
    //{
      //EntityProcessingSystem::renderEntities(bag);
    //};

    virtual void added(Entity &e) 
    {
      if (inited) return;
      inited = true;

      ofPixels& input = get_input(e);
      ofPixels& output = get_output(e);

      int input_w = input.getWidth();
      int input_h = input.getHeight();
      int w = (float)input_w * img_scale;
      int h = (float)input_h * img_scale;

      output.allocate( w, h, channels );
      output.set(0);
      output_img.allocate(w, h, OF_IMAGE_GRAYSCALE);
    }; 

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);

      if ( !depth_data->dirty )
        return;

      ofPixels& input = get_input(e);
      ofPixels& output = get_output(e);

      ofxCv::resize( input, output, img_scale, img_scale );

      //TODO gpu
      ofxCv::blur( output, blur_size );

      //TODO smooth con opencv:
      //1)
      //cv::dilate(image,result,cv::Mat());
      //cv::erode(result,result,cv::Mat());
      //cv::dilate(result,result,cv::Mat());
      //2)
      //cv::Mat element5(5,5,CV_8U,cv::Scalar(1));
      //cv::Mat closed;
      //cv::morphologyEx(image,closed,cv::MORPH_CLOSE,element5);
      //cv::Mat opened;
      //cv::morphologyEx(image,opened,cv::MORPH_OPEN,element5);

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

    float img_scale;
    int blur_size;
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

