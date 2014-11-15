#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "components/Components.h"
#include "ofxCamaraLucida.h"

using namespace artemis;

class CamaraLucidaSystem : public ECSsystem 
{ 

  public:

    CamaraLucidaSystem(string _id) : ECSsystem(_id)
    {
      cml_data = NULL;
      render_data = NULL;

      addComponentType<CamaraLucidaComponent>();
      addComponentType<RenderComponent>();
    };

    //~CamaraLucidaSystem()
    //{ dispose(); };

    virtual void initialize() 
    {
      cml_m.init( *world );
      render_m.init( *world );
    };

    void dispose()
    {
      if ( cml_data != NULL ) 
        cml_data->enabled.removeListener(this, &CamaraLucidaSystem::update_render_data);
      cml_data = NULL;
      render_data = NULL;
    };

    virtual void added(Entity &e) 
    {
      if ( cml_data != NULL ) 
      {
        ofLogWarning("CamaraLucidaSystem") << "add entity: entity already exists and only 1 entity with CamaraLucidaComponent is expected";
        dispose();
      }

      cml_data = cml_m.get(e);
      render_data = render_m.get(e);

      cml_data->enabled.addListener(this, &CamaraLucidaSystem::update_render_data);

      bool enabled = cml_data->enabled;
      update_render_data( enabled );
    };

    virtual void removed(Entity &e) 
    {
      dispose();
    };

    virtual void processEntity(Entity &e) 
    {
      //update_render_data();

      DepthComponent* depth = component<DepthComponent>("input");

      if ( depth->dirty )
      {
        cml_data->cml->update( depth->depth_pix_mm );
      }
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      int len = bag.getCount();

      if ( len == 1 )
        processEntity( *bag.get(0) );

      else if ( len > 1 )
        ofLogError("CamaraLucidaSystem") << "expected only 1 entity with CamaraLucidaComponent and got " << len;

      //for ( int i = 0; i < len; i++ )
        //processEntity( *bag.get(i) );
    };

    virtual void render_entity(Entity &e)
    {
      if ( cml_data->render_background )
      {
        ofPushStyle();
        ofColor(255,255,255);
        ofRect( 0, 0, render_data->width, render_data->height );
        ofPopStyle();
      }

      if ( cml_data->render_hue_tex )
      {
        render_hue_tex(); 
      }
    };

    void update_render_data(bool& enabled)
    { 
      //ofLogNotice("CamaraLucidaSystem") << "update_render_data, cml enabled: " << enabled;

      float w, h;

      if ( enabled )
      {
        w = cml_data->cml->tex_width();
        h = cml_data->cml->tex_height();
      }
      else 
      {
        w = ofGetWidth();
        h = ofGetHeight();
      }

      render_data->update( w, h );
    };

  private:

    void render_hue_tex()
    {
      DepthComponent* depth = component<DepthComponent>("input");

      int w = render_data->width;
      int h = render_data->height;

      ofSetColor(255);
      cml_data->cml->depth_camera()->get_hue_tex_ref( depth->depth_pix_mm ).draw( 0, 0, w, h );
    };

    //single entity
    CamaraLucidaComponent* cml_data;
    RenderComponent* render_data;

    ComponentMapper<CamaraLucidaComponent> cml_m;
    ComponentMapper<RenderComponent> render_m;

};

