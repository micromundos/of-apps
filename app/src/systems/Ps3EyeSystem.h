#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxPS3EyeGrabber.h"
#include "ofxTimeMeasurements.h"

using namespace artemis;

class Ps3EyeSystem : public ECSsystem 
{ 

  public:

    Ps3EyeSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<RgbComponent>();
    };

    ~Ps3EyeSystem()
    {
      ps3.close();
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
        ofLogWarning("Ps3EyeSystem") 
          << "ps3 singleton entity already added";
        return;
      }
      entity = &e; 

      RgbComponent* rgb_data = rgb_m.get(e);

      int w = rgb_data->width;
      int h = rgb_data->height;

      rgb_data->setup( w, h );

      ps3.initGrabber(w, h);
      ps3.setDesiredFrameRate(30);
      ps3.setAutogain(true);
      ps3.setAutoWhiteBalance(true);

      //ps3.setGain(uint8_t val);
      //ps3.setExposure(uint8_t val);
      //ps3.setSharpness(uint8_t val);
      //ps3.setContrast(uint8_t val);
      //ps3.setBrightness(uint8_t val);
      //ps3.setHue(uint8_t val);
      //ps3.setRedBalance(uint8_t val);
      //ps3.setBlueBalance(uint8_t val);

      //test.loadImage("test_aruco_markers_640x480.jpg");
      //test.loadImage("test_aruco_markers_1280x720.jpg");
      //test.loadImage("test_aruco_markers_1920x1080.jpg");
    };

    virtual void removed(Entity &e) 
    {
      entity = NULL;
    };

    virtual void processEntity(Entity &e) 
    {
      if ( entity == NULL ) return;

      //TS_START("Ps3EyeSystem test aruco");
      //RgbComponent* rgb_data = rgb_m.get(e);
      //rgb_data->dirty = true;
      //rgb_data->pixels = &(test.getPixelsRef());
      //TS_STOP("Ps3EyeSystem test aruco");
      //return;

      TS_START("Ps3EyeSystem update ps3");
      ps3.update();
      TS_STOP("Ps3EyeSystem update ps3");

      RgbComponent* rgb_data = rgb_m.get(e);
      bool dirty = ps3.isFrameNew();
      rgb_data->dirty = dirty;
      if ( dirty )
      {
        //rgb_data->update( ps3.getPixelsRef() );
        TS_START("Ps3EyeSystem copy pixels");
        //copy & convert rgba -> rgb
        ps3_pix = ps3.getPixelsRef();
        ps3_pix.setNumChannels(3);
        rgb_data->pixels = &ps3_pix;
        TS_STOP("Ps3EyeSystem copy pixels");
      }
    }; 

    virtual void renderEntity(Entity &e)
    {
      RgbComponent* rgb_data = rgb_m.get(e);
      if ( !rgb_data->render )
        return;

      RenderComponent* render_data = require_component<RenderComponent>("output");

      ofPushStyle();
      ofSetColor(255); 

      ps3_tex.loadData( ps3.getPixelsRef() );
      ps3_tex.draw( 0, 0,
          rgb_data->width, 
          rgb_data->height );
          //render_data->width, 
          //render_data->height );

      //test.draw( 0, 0, 640, 480 );
      //test.draw( 0, 0, 640, 360 );

      ofPopStyle();
    };

  private:

    ComponentMapper<RgbComponent> rgb_m;

    ofxPS3EyeGrabber ps3;
    ofPixels ps3_pix;
    ofTexture ps3_tex; //render

    //ofImage test;

    Entity* entity;
};

