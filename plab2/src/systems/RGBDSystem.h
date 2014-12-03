#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "components/Components.h"
#include "ofxKinect.h"

using namespace artemis;

class RGBDSystem : public ECSsystem 
{ 

  public:

    RGBDSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<RGBComponent>();
      addComponentType<DepthComponent>();
    };

    ~RGBDSystem()
    {
      kinect.close();
    };

    virtual void initialize() 
    {
      rgb_m.init( *world );
      depth_m.init( *world );
      inited = false; 
    };

    virtual void added(Entity &e) 
    { 
      init(e);
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      kinect.update();
      for (int i=0;i<bag.getCount();i++)
        processEntity( *bag.get(i) ); 
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("RGBDSystem") << "process entity " << e.getId();

      DepthComponent* depth_data = depth_m.get(e);
      RGBComponent* rgb_data = rgb_m.get(e);

      bool dirty = kinect.isFrameNew();
      depth_data->dirty = dirty; 
      rgb_data->dirty = dirty; 

      if ( dirty )
      {
        depth_data->update( kinect.getRawDepthPixels() );
        depth_data->update( kinect.getDepthPixels() );
        //depth_data->update( kinect.getDepthTextureReference() );
        rgb_data->update( kinect.getPixels() );
      }

    };

    virtual void render_entity(Entity &e)
    {

      RenderComponent* render_data = component<RenderComponent>("output");
      int w = render_data->width;
      int h = render_data->height;

      ofSetColor(255);

      if ( rgb_m.get(e)->render )
        kinect.draw( 0, 0, w, h );

      if ( depth_m.get(e)->render )
        kinect.drawDepth( 0, 0, w, h ); 

    };

  private: 

    ComponentMapper<RGBComponent> rgb_m;
    ComponentMapper<DepthComponent> depth_m;

    ofxKinect kinect; 
    bool inited;


    void init( Entity &e )
    {
      if (inited)
      {
        ofLogWarning("RGBDSystem") << "calling init but kinect is already inited";
        return;
      }
      inited = true; 

      DepthComponent* depth_data = depth_m.get(e);
      RGBComponent* rgb_data = rgb_m.get(e);

      int w = kinect.width;
      int h = kinect.height;

      rgb_data->setup( w, h );
      depth_data->setup( w, h );

      //registration transforms rgb pixels to match depth pixels, not the other way around, right? then we're fine!
      kinect.setRegistration(true);
      kinect.enableDepthNearValueWhite(true);
      // ir, rgb, texture
      kinect.init( rgb_data->ir, true, true );
      kinect.open();
    };
};

