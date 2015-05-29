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
      inited = false; 
    };

    virtual void added(Entity &e) 
    {
      if (inited)
      {
        ofLogWarning("Ps3EyeSystem") << "ps3 already inited";
        return;
      }
      inited = true; 

      RgbComponent* rgb_data = rgb_m.get(e);

      int w = rgb_data->width;
      int h = rgb_data->height;

      rgb_data->setup( w, h );
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
      ps3.initGrabber(w, h);
    };

    virtual void processEntity(Entity &e) 
    {
      if (!inited) return;

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
        rgb_data->update( ps3_pix );
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

      ofPopStyle();
    };

  private:

    ComponentMapper<RgbComponent> rgb_m;

    ofxPS3EyeGrabber ps3;
    ofPixels ps3_pix;
    ofTexture ps3_tex; //render
    bool inited;

};

