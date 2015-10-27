#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxTimeMeasurements.h"

using namespace artemis;

class VideoInputSystem : public ECSsystem 
{ 

  public:

    VideoInputSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<RgbComponent>();
    };

    virtual void initialize() 
    {
      rgb_m.init( *world );
      entity = NULL; 
    };

    virtual void added(Entity &e) 
    {
      if (entity != NULL)
      {
        ofLogWarning("VideoInputSystem") 
          << "singleton entity already added";
        return;
      }
      entity = &e; 

      RgbComponent* rgb_data = rgb_m.get(e);
      int w = rgb_data->width;
      int h = rgb_data->height;
      rgb_data->setup(w,h);

      video_grabber.setDeviceID(1);
      video_grabber.initGrabber(w,h,true);
    }; 

    virtual void removed(Entity &e) 
    {
      entity = NULL;
    };

    virtual void processEntity(Entity &e) 
    {
      video_grabber.update();

      RgbComponent* rgb_data = rgb_m.get(e);
      bool dirty = video_grabber.isFrameNew();
      rgb_data->dirty = dirty;

      if ( dirty )
      {
        //rgb_data->pixels = &(video_grabber.getPixelsRef());
        //copy & scale
        pix = video_grabber.getPixelsRef();
        //float scale = 1.0;
        //int w = rgb_data->width * scale;
        //int h = rgb_data->height * scale;
        //pix.resize(w,h);
        int w = rgb_data->width;
        int h = rgb_data->height;
        pix.crop( 
            (float)w*0.25, (float)h*0.25,
            (float)w*0.5, (float)h*0.5 );

        rgb_data->pixels = &(pix);
      }
    }; 

    virtual void renderEntity(Entity &e)
    {
      RgbComponent* rgb_data = rgb_m.get(e);

      if ( !rgb_data->render )
        return;

      //RenderComponent* render_data = component<RenderComponent>("output");

      ofPushStyle();
      ofSetColor(255);

      video_grabber.draw(0,0,
          rgb_data->width, 
          rgb_data->height );
          //render_data->width,
          //render_data->height );

      ofPopStyle();
    };

  private:

    ComponentMapper<RgbComponent> rgb_m;

    ofVideoGrabber video_grabber;
    ofPixels pix;

    Entity* entity;
};

