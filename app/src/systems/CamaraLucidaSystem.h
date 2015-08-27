#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxCamaraLucida.h"
#include "ofxTimeMeasurements.h"
#include "keys.h"

using namespace artemis;

class CamaraLucidaSystem : public ECSsystem 
{ 

  public:

    CamaraLucidaSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<CamaraLucidaComponent>();
      addComponentType<RenderComponent>();
      cml_data = NULL;
      render_data = NULL;
    };

    //~CamaraLucidaSystem()
    //{ dispose(); };

    virtual void initialize() 
    {
      camara_lucida_m.init( *world );
      render_m.init( *world );
      inited = false; 
    };

    void dispose()
    {
      if ( cml_data != NULL ) 
      {
        cml_data->enabled.removeListener(this, &CamaraLucidaSystem::update_render_data);
        ofRemoveListener(ofEvents().keyPressed, this, &CamaraLucidaSystem::keyPressed);
      }
      cml_data = NULL;
      render_data = NULL;
      inited = false; 
    };

    virtual void added(Entity &e) 
    {
      if (inited)
      {
        ofLogWarning("CamaraLucidaSystem") 
          << "singleton entity already added";
        return;
      }
      inited = true;

      cml_data = camara_lucida_m.get(e);
      render_data = render_m.get(e);

      initial_projector_frustum = cml_data->cml->projector()->gl_frustum();
      cml_data->enabled.addListener(this, &CamaraLucidaSystem::update_render_data);

      bool enabled = cml_data->enabled;
      update_render_data( enabled );

      ofAddListener(ofEvents().keyPressed, this, &CamaraLucidaSystem::keyPressed);
    };

    virtual void removed(Entity &e) 
    {
      dispose();
    }; 

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth_data = require_component<DepthComponent>("input");

      cml_data->cml->depth_camera()->xoff = cml_data->xoff;

      if ( !depth_data->dirty )
        return;

      TS_START("CamaraLucidaSystem");
      cml_data->cml->update( depth_data->depth_ofpix_mm->getPixels() );
      TS_STOP("CamaraLucidaSystem");
    };

    void update_render_data(bool& enabled)
    { 
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
      float bg = cml_data->render_background;
      if ( bg > 0.0 )
      {
        ofPushStyle();
        ofSetColor( bg*255 );
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

    void keyPressed(ofKeyEventArgs &args)
    {
      if (cml_data == NULL) return;

      cml::CamaraLucida* cml = cml_data->cml;

      if ( args.key == keys::cml_tweak_frustum_inc || 
          args.key == keys::cml_tweak_frustum_dec )
      {
        cml::OpticalDevice::Frustum& f = cml->projector()->gl_frustum();
        float d = 0.1 * (args.key == keys::cml_tweak_frustum_inc ? 1 : -1);
        f.left -= d;
        f.right += d;
        f.bottom -= d;
        f.top += d;
      }

      if ( args.key == keys::cml_tweak_frustum_reset )
      {
        cml::OpticalDevice::Frustum& f = cml->projector()->gl_frustum();
        f.left = initial_projector_frustum.left;
        f.right = initial_projector_frustum.right;
        f.bottom = initial_projector_frustum.bottom;
        f.top = initial_projector_frustum.top;
      }

      if ( args.key == keys::cml_gpu )
      {
        cml->gpu( !cml->gpu() );
      }

      if ( args.key == keys::cml_wireframe )
      {
        cml->wireframe( !cml->wireframe() );
      }
    };

  private:

    bool inited;
    CamaraLucidaComponent* cml_data;
    RenderComponent* render_data;
    cml::OpticalDevice::Frustum initial_projector_frustum;

    ComponentMapper<CamaraLucidaComponent> camara_lucida_m;
    ComponentMapper<RenderComponent> render_m;

    void render_hue_tex()
    {
      if (cml_data == NULL) return;

      DepthComponent* depth_data = component<DepthComponent>("input");

      int w = render_data->width;
      int h = render_data->height;

      ofSetColor(255);
      cml_data->cml->depth_camera()->get_hue_tex_ref( depth_data->depth_ofpix_mm->getPixels() ).draw( 0, 0, w, h );
    }; 

};

