#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxKinect.h"
#include "ofxCv.h"
#include "ofxTimeMeasurements.h"

using namespace artemis;

class KinectSystem : public ECSsystem 
{ 

  public:

    KinectSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<DepthComponent>();
    };

    ~KinectSystem()
    {};

    virtual void initialize() 
    {
      depth_m.init( *world );
      inited = false; 
      tprev = 0;
      angle_prev = 0;
    };

    virtual void removed(Entity &e) 
    {
      ofLogNotice("KinectSystem") 
        << "kinect.close()";
      depth_m.get(e)->kinect.close();
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
      ofxKinect& kinect = depth_data->kinect;

      int w = kinect.width;
      int h = kinect.height;

      depth_data->setup( w, h );

      kinect.setRegistration(false);
      kinect.enableDepthNearValueWhite(true);
      // ir, rgb, texture
      kinect.init( false, false, true );
      kinect.open();
    };

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);
      ofxKinect& kinect = depth_data->kinect;

      TS_START("KinectSystem update kinect");
      kinect.update();
      TS_STOP("KinectSystem update kinect");

      int angle = depth_data->angle;
      if ( angle != angle_prev )
        kinect.setCameraTiltAngle( angle );
      angle_prev = angle;

      bool dirty = kinect.isFrameNew();
      depth_data->dirty = dirty; 

      if ( !dirty ) return;

      //calc_fps();

      if ( !depth_data->flip )
      {
        depth_data->depth_ofpix_mm = &(kinect.getRawDepthPixelsRef());
        depth_data->depth_ofpix_grey = &(kinect.getDepthPixelsRef());
        depth_data->f_depth_ofpix_mm = &(kinect.getDistancePixelsRef());
        depth_data->f_depth_img.setFromPixels( kinect.getDistancePixelsRef() );
      }

      else
      {
        TS_START("KinectSystem flip");

        int mode = -1;
        ofxCv::flip( kinect.getRawDepthPixelsRef(), depth_ofpix_mm, mode );
        ofxCv::flip( kinect.getDepthPixelsRef(), depth_ofpix_grey, mode );
        ofxCv::flip( kinect.getDistancePixelsRef(), f_depth_ofpix_mm, mode );

        depth_data->depth_ofpix_mm = &depth_ofpix_mm;
        depth_data->depth_ofpix_grey = &depth_ofpix_grey;
        depth_data->f_depth_ofpix_mm = &f_depth_ofpix_mm;
        depth_data->f_depth_img.setFromPixels( f_depth_ofpix_mm );

        TS_STOP("KinectSystem flip");
      }

    };

    virtual void renderEntity(Entity &e)
    {
      DepthComponent* depth_data = depth_m.get(e);
      ofxKinect& kinect = depth_data->kinect;

      CamaraLucidaComponent* cml_data = component<CamaraLucidaComponent>("output");

      RenderComponent* render_data = component<RenderComponent>("output");
      int w = render_data->width;
      int h = render_data->height;

      ofPushStyle();
      ofSetColor(255);

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

    ComponentMapper<DepthComponent> depth_m;

    bool inited;
    float fps, tprev;
    int angle_prev;

    //flip tmp
    ofShortPixels depth_ofpix_mm; //uint16_t
    ofFloatPixels f_depth_ofpix_mm; //float
    ofPixels depth_ofpix_grey; //uint8_t

    void calc_fps()
    {
      float tnow = ofGetElapsedTimeMillis();
      float FR = 1000.0/(tnow - tprev);
      float fA = 0.95; 
      float fB = 1.0-fA;
      fps = (fA*fps) + (fB*FR); 
      tprev = tnow;
    };

};

