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

      RenderComponent* render_data = component<RenderComponent>("output");
      render( render_data->width, render_data->height ); 
    };

    // entity: escena
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

  private: 

    ComponentMapper<RGBComponent> rgb_m;
    ComponentMapper<DepthComponent> depth_m;

    ofxKinect kinect;

    void render( int render_width, int render_height )
    {
      ofSetColor(255);
      int w = render_width;
      int h = render_height;
      render_depth(0, 0, w, h);
      //render_color(0, 0, w, h);
    };

    void render_color( int x, int y, int w, int h )
    {
      kinect.draw( x, y, w, h );
    };

    void render_depth( int x, int y, int w, int h )
    {
      kinect.drawDepth( x, y, w, h );
    };

};

