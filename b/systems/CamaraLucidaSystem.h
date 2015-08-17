#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
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
      camara_lucida_m.init( *world );
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

      cml_data = camara_lucida_m.get(e);
      render_data = render_m.get(e);

      cml_data->enabled.addListener(this, &CamaraLucidaSystem::update_render_data);

      bool enabled = cml_data->enabled;
      update_render_data( enabled );
    };

    virtual void removed(Entity &e) 
    {
      dispose();
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

    virtual void processEntity(Entity &e) 
    {

      DepthComponent* depth_data = require_component<DepthComponent>("input");

      cml_data->cml->depth_camera()->xoff = cml_data->xoff;

      if ( !depth_data->dirty )
        return;

      cml_data->cml->update( depth_data->depth_ofpix_mm->getPixels() );
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

    virtual void renderEntity(Entity &e)
    {
      if ( cml_data == NULL )
        return;

      //CamaraLucidaComponent* cml_data = camara_lucida_m.get(e);
      //RenderComponent* render_data = render_m.get(e);

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

    void depth_to_p3( const ofVec2f& p2, ofVec3f& p3 )
    {
      if (cml_data == NULL) return;

      DepthComponent* depth_data = require_component<DepthComponent>("input");

      float far_clamp = cml_data->cml->depth_camera()->far_clamp;
      float epsilon = std::numeric_limits<float>::epsilon();

      uint16_t* depth_pix_mm = depth_data->depth_ofpix_mm->getPixels();
      int i = (int)p2.y * depth_data->width + (int)p2.x;
      uint16_t mm = depth_pix_mm[i];

      mm = CLAMP( ( mm < epsilon ? far_clamp : mm ), 0.0, far_clamp );

      float x,y;
      cml_data->cml->depth_camera()->unproject( p2.x, p2.y, mm, &x, &y );
      p3.set( x, y, mm ); //XXX ofScale(-1,-1,1);

      if ( !depth_data->flip )
      {
        p3.x *= -1;
        p3.y *= -1;
      }
    };

  private:

    void render_hue_tex()
    {
      if (cml_data == NULL) return;

      DepthComponent* depth_data = component<DepthComponent>("input");

      int w = render_data->width;
      int h = render_data->height;

      ofSetColor(255);
      cml_data->cml->depth_camera()->get_hue_tex_ref( depth_data->depth_ofpix_mm->getPixels() ).draw( 0, 0, w, h );
    };

    //single entity
    //save the components to use 
    //in the event listener
    //TODO a better way to do this?
    CamaraLucidaComponent* cml_data;
    RenderComponent* render_data;

    ComponentMapper<CamaraLucidaComponent> camara_lucida_m;
    ComponentMapper<RenderComponent> render_m;

};

