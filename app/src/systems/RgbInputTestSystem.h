#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

using namespace artemis;

class RgbInputTestSystem : public ECSsystem 
{ 

  public:

    RgbInputTestSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<RgbComponent>();
    };

    virtual void initialize() 
    {
      rgb_m.init( *world );
    };

    virtual void added(Entity &e) 
    {
      //img.loadImage("test_aruco_markers_640x480.jpg");
      //img.loadImage("test_aruco_markers_1280x720.jpg");
      img.loadImage("test_aruco_markers_1920x1080.jpg");
      //img.loadImage("test_aruco_markers_1280x720_crop640x360.jpg");
    }; 

    virtual void processEntity(Entity &e) 
    {
      TS_START("RgbInputTestSystem test aruco");

      RgbComponent* rgb_data = rgb_m.get(e);
      rgb_data->dirty = true;

      int w = 1920;
      int h = 1080;
      img.crop( 
          0, 0,
          (float)w*0.5, (float)h*0.5 );

      rgb_data->pixels = &(img.getPixelsRef());

      TS_STOP("RgbInputTestSystem test aruco");
    }; 

    virtual void renderEntity(Entity &e)
    {
      //img.draw( 0, 0, 640, 480 ); //4:3
      img.draw( 0, 0, 640, 360 ); //16:9
    };

  private:

    ComponentMapper<RgbComponent> rgb_m;

    ofImage img;
};

