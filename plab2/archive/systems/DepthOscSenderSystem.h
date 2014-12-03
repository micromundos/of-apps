#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "components/Components.h"
#include "ofxOsc.h"

using namespace artemis;

//TODO try ofFloatPixels ofFloatImage

//TODO refactor to 
//plane_calib_osc
//PlaneCalibOscSystem
//PlaneCalibOscComponent
//listen to calib result on osc receiver

class DepthOscSenderSystem : public ECSsystem 
{ 

  public:

    DepthOscSenderSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<DepthOscSenderComponent>();
      addComponentType<DepthComponent>();
    };

    virtual void initialize() 
    {
      depth_osc_sender_m.init( *world );
      depth_m.init( *world );
      running = false;
    };

    virtual void added(Entity &e) 
    {
      init_dpix(e);
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("DepthOscSenderSystem") << "process entity " << e.getId();

      bool run = depth_osc_sender_m.get(e)->run;

      if ( run && !running )
      {
        running = true;
      }
      else if ( run && running && !sent )
      {
        sent = send_depth(e);
      }
      else if ( !run && running )
      {
        running = false;
        sent = false;
      }

    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      int len = bag.getCount();
      for ( int i = 0; i < len; i++ )
        processEntity( *bag.get(i) );
    };

    virtual void render_entity(Entity &e)
    {};

  private:

    bool running;
    bool sent;

    //ofShortPixels dpix; //mm
    ofPixels dpix; //grey
    ofImage dimg;
    int channels;

    void init_dpix(Entity &e)
    {
      if ( dpix.isAllocated() )
        return;

      DepthComponent* depth_data = depth_m.get(e);

      int w = depth_data->width;
      int h = depth_data->height;
      channels = 1;
      dpix.allocate( w, h, channels );
      //dimg.allocate( w, h, OF_IMAGE_GRAYSCALE );
    };

    bool send_depth(Entity &e)
    {
      DepthComponent* depth_data = depth_m.get(e);
      if ( !depth_data->dirty )
        return false;

      ofxOscSender& sender = depth_osc_sender_m.get(e)->sender;

      int w = depth_data->width; 
      int h = depth_data->height; 
      //size_t len = w * h * sizeof(char);
      size_t len = w * h * sizeof(uint8_t);
      //unsigned int len = w * h;

      //float* depth_pix = depth_data->depth_pix_mm_f; 
      //uint16_t* depth_pix = depth_data->depth_pix_mm; 
      //uint8_t* depth_pix = depth_data->depth_pix_grey; 
      //uint8_t* depth_pix = new uint8_t[len];
      //memcpy( depth_pix, depth_data->depth_pix_grey, len );

      //dpix.setFromPixels( depth_pix, w, h, channels ); 
      //dimg.setFromPixels( dpix );
      //dimg.setFromPixels( depth_pix, w, h, OF_IMAGE_GRAYSCALE ); 

      ofTexture& depth_tex = depth_data->depth_tex;
      depth_tex.readToPixels( dpix );

      //ofBuffer dbuf( reinterpret_cast<const char*>(depth_pix), len );
      ofBuffer dbuf( reinterpret_cast<const char*>(dpix.getPixels()), dpix.size() );

      //dimg.loadImage( dbuf );

      ofLog() << "send depth"
        << ", buf size: " << dbuf.getText().size()
        << ", pix size: " << dpix.size();

      ofxOscMessage m;
      m.setAddress("/depth");
      m.addBlobArg( dbuf );
      sender.sendMessage(m);

      //delete depth_pix;

      return true;
    };

    ComponentMapper<DepthOscSenderComponent> depth_osc_sender_m;
    ComponentMapper<DepthComponent> depth_m;

};

