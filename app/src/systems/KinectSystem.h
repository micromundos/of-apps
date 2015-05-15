#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxKinect.h"
#include "ofxCv.h"

using namespace artemis;

class KinectSystem : public ECSsystem 
{ 

  public:

    KinectSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<DepthComponent>();
      //addComponentType<RgbComponent>();
    };

    ~KinectSystem()
    {
      ofLogNotice("KinectSystem") << "kinect.close()";
      kinect.close();
    };

    virtual void initialize() 
    {
      rgb_m.init( *world );
      depth_m.init( *world );
      inited = false; 
      prev = 0;
    };

    virtual void added(Entity &e) 
    { 
      if (inited)
      {
        ofLogWarning("KinectSystem") << "kinect is already inited";
        return;
      }
      inited = true; 

      DepthComponent* depth_data = depth_m.get(e);
      //RgbComponent* rgb_data = rgb_m.get(e);

      int w = kinect.width;
      int h = kinect.height;

      //rgb_data->setup( w, h );
      depth_data->setup( w, h );

      kinect.setRegistration(false);
      kinect.enableDepthNearValueWhite(true);
      // ir, rgb, texture
      kinect.init( false, false, true );
      kinect.open();

    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("KinectSystem") << "process entity " << e.getId();

      kinect.update();

      DepthComponent* depth_data = depth_m.get(e);
      //RgbComponent* rgb_data = rgb_m.get(e);

      bool dirty = kinect.isFrameNew();
      depth_data->dirty = dirty; 
      //rgb_data->dirty = dirty; 

      if ( !dirty ) return;

      //calc_fps();

      if ( !depth_data->flip )
      {
        depth_data->update( kinect.getRawDepthPixelsRef() );
        depth_data->update( kinect.getDistancePixelsRef() );
        depth_data->update( kinect.getDepthPixelsRef() );
      }

      else
      {
        int mode = -1;
        ofxCv::flip( kinect.getRawDepthPixelsRef(), depth_ofpix_mm, mode );
        ofxCv::flip( kinect.getDistancePixelsRef(), f_depth_ofpix_mm, mode );
        ofxCv::flip( kinect.getDepthPixelsRef(), depth_ofpix_grey, mode );

        depth_data->update( depth_ofpix_mm );
        depth_data->update( f_depth_ofpix_mm );
        depth_data->update( depth_ofpix_grey );
      }
    };

    virtual void renderEntity(Entity &e)
    {
      DepthComponent* depth_data = depth_m.get(e);

      CamaraLucidaComponent* cml_data = require_component<CamaraLucidaComponent>("output");

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int w = render_data->width;
      int h = render_data->height;

      ofPushStyle();
      ofSetColor(255);

      if ( rgb_m.get(e)->render )
        kinect.draw( 0, 0, w, h );

      if ( depth_data->render )
      {
        if ( depth_data->flip && cml_data->enabled )
          kinect.drawDepth( w, h, -w, -h ); 
        else
          kinect.drawDepth( 0, 0, w, h ); 
      }

      ofPopStyle();

    };

  private: 

    ComponentMapper<RgbComponent> rgb_m;
    ComponentMapper<DepthComponent> depth_m;

    ofxKinect kinect; 
    bool inited;
    float fps, prev;

    //flip
    ofShortPixels depth_ofpix_mm;//uint16_t
    ofFloatPixels f_depth_ofpix_mm;//float
    ofPixels depth_ofpix_grey;//uint8_t

    void calc_fps()
    {
      float now = ofGetElapsedTimeMillis();
      float FR = 1000.0/(now - prev);
      float fA = 0.95; 
      float fB = 1.0-fA;
      fps = (fA*fps) + (fB*FR); 
      prev = now;
    };

};

