#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/Components.h"
#include "ofxKinect.h"

using namespace artemis;

class RGBDSystem : public ECSsystem 
{ 

  public:

    RGBDSystem() 
    {
      addComponentType<RGBComponent>();
      addComponentType<DepthComponent>();
    };

    virtual void initialize() 
    {
      ofLogNotice("RGBDSystem") << "init";

      rgb_m.init( *world );
      depth_m.init( *world );

      //registration transforms rgb pixels to match depth pixels, not the other way around, right? then we're fine!
      kinect.setRegistration(true);
      //kinect.enableDepthNearValueWhite(false);
      // ir, rgb, texture
      kinect.init(false, true, true);
      kinect.open();
    };

    virtual void added(Entity &e) 
    {
      int w = kinect.width;
      int h = kinect.height;
      rgb_m.get(e)->init( w, h );
      depth_m.get(e)->init( w, h );
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

      bool dirty = kinect.isFrameNew();
      depth_m.get(e)->dirty = dirty; 
      rgb_m.get(e)->dirty = dirty; 

      if ( dirty )
      {
        depth_m.get(e)->update( kinect.getRawDepthPixels() );
        depth_m.get(e)->update( kinect.getDepthPixels() );
        rgb_m.get(e)->update( kinect.getPixels() );
      }

    };

    virtual void render()
    {
      RenderComponent* render_data = component<RenderComponent>("output");
      int w = render_data->width;
      int h = render_data->height;

      ofSetColor(255);
      //kinect.drawDepth( 0, 0, w, h );
      kinect.draw( 0, 0, w, h );
    };

  private: 

    ComponentMapper<RGBComponent> rgb_m;
    ComponentMapper<DepthComponent> depth_m;

    ofxKinect kinect; 
};

