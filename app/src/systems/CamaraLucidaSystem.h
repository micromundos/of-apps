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
        ofRemoveListener(ofEvents().keyReleased, this, &CamaraLucidaSystem::keyReleased);
      }

      cml_data->tweak_load.removeListener( this, &CamaraLucidaSystem::tweak_load );
      cml_data->tweak_save.removeListener( this, &CamaraLucidaSystem::tweak_save );
      cml_data->tweak_reset.removeListener( this, &CamaraLucidaSystem::tweak_reset );

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

      tweak_init();

      for (int i = 0; i < 512; i++) 
        pressed[i] = false;

      cml_data->enabled.addListener(this, &CamaraLucidaSystem::update_render_data);

      bool enabled = cml_data->enabled;
      update_render_data( enabled );

      ofAddListener(ofEvents().keyPressed, this, &CamaraLucidaSystem::keyPressed);
      ofAddListener(ofEvents().keyReleased, this, &CamaraLucidaSystem::keyReleased);
    };

    virtual void removed(Entity &e) 
    {
      dispose();
    }; 

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth_data = component<DepthComponent>("input");

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

      DepthComponent* depth_data = component<DepthComponent>("input");

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

    void keyReleased(ofKeyEventArgs &args)
    {
      pressed[args.key] = false;
    };

    void keyPressed(ofKeyEventArgs &args)
    {
      pressed[args.key] = true;

      if (cml_data == NULL) 
        return;

      cml::CamaraLucida* cml = cml_data->cml;
      cml::OpticalDevice* proj = cml->projector();

      if ( pressed[keys::cml_tweak_frustum] 
        && (args.key == OF_KEY_UP 
        || args.key == OF_KEY_DOWN) )
      {
        tweak_frustum_y( args.key == OF_KEY_DOWN ? 1. : -1. );
      } 

      if ( pressed[keys::cml_tweak_frustum] 
        && (args.key == OF_KEY_LEFT 
        || args.key == OF_KEY_RIGHT) )
      {
        tweak_frustum_x( args.key == OF_KEY_RIGHT ? 1. : -1. );
      }

      if ( pressed[keys::cml_tweak_Rz] 
        && (args.key == OF_KEY_UP 
        || args.key == OF_KEY_DOWN) )
      {
        tweak_Rz( args.key == OF_KEY_UP ? 1. : -1. );
      }

      //FIXME
      //if ( pressed[keys::cml_tweak_Rxy] 
        //&& (args.key == OF_KEY_UP 
        //|| args.key == OF_KEY_DOWN) )
      //{
        //tweak_Rx( args.key == OF_KEY_UP ? 1. : -1. );
      //}

      //if ( pressed[keys::cml_tweak_Rxy] 
        //&& (args.key == OF_KEY_LEFT 
        //|| args.key == OF_KEY_RIGHT) )
      //{
        //tweak_Ry( args.key == OF_KEY_RIGHT ? 1. : -1. );
      //}

      if ( pressed[keys::cml_tweak_Txy] 
        && (args.key == OF_KEY_LEFT 
        || args.key == OF_KEY_RIGHT) )
      {
        tweak_Tx( args.key == OF_KEY_LEFT ? 1. : -1. );
      }

      if ( pressed[keys::cml_tweak_Txy] 
        && (args.key == OF_KEY_UP 
        || args.key == OF_KEY_DOWN) )
      {
        tweak_Ty( args.key == OF_KEY_UP ? 1. : -1. );
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

    //tweak projector
    cml::OpticalDevice::Frustum tweak_ini_frustum;
    ofVec3f tweak_ini_loc;
    ofVec3f tweak_ini_fwd;
    ofVec3f tweak_ini_up;
    ofVec3f tweak_ini_trg;
    ofVec3f tweak_ini_left;
    float tweak_T_off;
    float tweak_R_off;
    float tweak_frustum_off;
    string tweak_file;

    bool pressed[512];

    ComponentMapper<CamaraLucidaComponent> camara_lucida_m;
    ComponentMapper<RenderComponent> render_m;

    void tweak_init()
    {
      cml_data->tweak_load.addListener( this, &CamaraLucidaSystem::tweak_load );
      cml_data->tweak_save.addListener( this, &CamaraLucidaSystem::tweak_save );
      cml_data->tweak_reset.addListener( this, &CamaraLucidaSystem::tweak_reset );

      cml::OpticalDevice* proj = cml_data->cml->projector();

      tweak_ini_frustum = proj->gl_frustum();
      tweak_ini_loc = proj->loc();
      tweak_ini_fwd = proj->fwd();
      tweak_ini_up = proj->up();
      tweak_ini_trg = proj->trg();
      tweak_ini_left = proj->left();

      tweak_frustum_off = 0.1;
      tweak_R_off = 0.1;
      tweak_T_off = 0.0005;

      tweak_file = "calib/cml_tweak.yml";

      bool b = true;
      tweak_load(b);
    };

    void tweak_reset(bool& enabled)
    {
      if (!enabled) return;

      cml::OpticalDevice* proj = cml_data->cml->projector();
      cml::OpticalDevice::Frustum& f = proj->gl_frustum();

      f.left = tweak_ini_frustum.left;
      f.right = tweak_ini_frustum.right;
      f.bottom = tweak_ini_frustum.bottom;
      f.top = tweak_ini_frustum.top;

      proj->loc( tweak_ini_loc );
      proj->fwd( tweak_ini_fwd );
      proj->up( tweak_ini_up );
      proj->trg( tweak_ini_trg );
      proj->left( tweak_ini_left );
    };

    void tweak_save(bool& enabled)
    {
      if (!enabled) return;

      ofLogNotice("CamaraLucidaSystem")
        << "save tweak to " << tweak_file;

      cml::OpticalDevice* proj = cml_data->cml->projector();
      cml::OpticalDevice::Frustum& f = proj->gl_frustum();

      cv::FileStorage fs( ofToDataPath(tweak_file, false), cv::FileStorage::WRITE ); 

      fs << "proj_frustum_left" << f.left;
      fs << "proj_frustum_right" << f.right;
      fs << "proj_frustum_bottom" << f.bottom;
      fs << "proj_frustum_top" << f.top;

      fs << "proj_loc_x" << proj->loc().x;
      fs << "proj_loc_y" << proj->loc().y;
      fs << "proj_loc_z" << proj->loc().z;

      fs << "proj_fwd_x" << proj->fwd().x;
      fs << "proj_fwd_y" << proj->fwd().y;
      fs << "proj_fwd_z" << proj->fwd().z;

      fs << "proj_up_x" << proj->up().x;
      fs << "proj_up_y" << proj->up().y;
      fs << "proj_up_z" << proj->up().z;

      fs << "proj_trg_x" << proj->trg().x;
      fs << "proj_trg_y" << proj->trg().y;
      fs << "proj_trg_z" << proj->trg().z;

      fs << "proj_left_x" << proj->left().x;
      fs << "proj_left_y" << proj->left().y;
      fs << "proj_left_z" << proj->left().z;
    };

    void tweak_load(bool& enabled)
    {
      if (!enabled) return;

      ofLogNotice("CamaraLucidaSystem")
        << "load tweak from " << tweak_file;

      cv::FileStorage fs( ofToDataPath(tweak_file, false), cv::FileStorage::READ ); 

      if ( !fs.isOpened() )
      {
        ofLogError("CamaraLucidaSystem") << "failed to load projector tweak file " << tweak_file;
        return false;
      }

      cml::OpticalDevice* proj = cml_data->cml->projector();
      cml::OpticalDevice::Frustum& f = proj->gl_frustum(); 

      f.left = fs["proj_frustum_left"];
      f.right = fs["proj_frustum_right"];
      f.bottom = fs["proj_frustum_bottom"];
      f.top = fs["proj_frustum_top"];

      ofVec3f ploc = ofVec3f( 
          fs["proj_loc_x"],
          fs["proj_loc_y"],
          fs["proj_loc_z"]);
      proj->loc( ploc );

      ofVec3f pfwd = ofVec3f( 
          fs["proj_fwd_x"],
          fs["proj_fwd_y"],
          fs["proj_fwd_z"]);
      proj->fwd( pfwd );

      ofVec3f pup = ofVec3f( 
          fs["proj_up_x"],
          fs["proj_up_y"],
          fs["proj_up_z"]); 
      proj->up( pup );

      ofVec3f ptrg = ofVec3f( 
          fs["proj_trg_x"],
          fs["proj_trg_y"],
          fs["proj_trg_z"]); 
      proj->trg( ptrg );

      ofVec3f pleft = ofVec3f( 
          fs["proj_left_x"],
          fs["proj_left_y"],
          fs["proj_left_z"]); 
      proj->left( pleft );

      ofLogNotice("CamaraLucidaSystem")
        << "loaded projector tweak from " << tweak_file << "\n";

      return true;
    };

    void tweak_frustum_x( float sign )
    {
      cml::OpticalDevice* proj = cml_data->cml->projector();
      cml::OpticalDevice::Frustum& f = proj->gl_frustum();

      float d = tweak_frustum_off * sign;
      f.left -= d;
      f.right += d;
    };

    void tweak_frustum_y( float sign )
    {
      cml::OpticalDevice* proj = cml_data->cml->projector();
      cml::OpticalDevice::Frustum& f = proj->gl_frustum();

      float d = tweak_frustum_off * sign;
      f.bottom -= d;
      f.top += d;
    };

    void tweak_Rz( float sign )
    {
      cml::OpticalDevice* proj = cml_data->cml->projector();

      ofVec3f& pivot = proj->fwd();
      float d = tweak_R_off * sign;

      ofVec3f left_rot = proj->left().getRotated( d, pivot );
      ofVec3f up_rot = proj->up().getRotated( d, pivot );

      proj->up( up_rot );
      proj->left( left_rot );
    };

    //FIXME
    //void tweak_Rx( float sign )
    //{
      //cml::OpticalDevice* proj = cml_data->cml->projector();

      //ofVec3f& pivot = proj->left();
      //float d = tweak_R_off * sign;

      //ofVec3f fwd_rot = proj->fwd().getRotated( d, pivot );
      //ofVec3f up_rot = proj->up().getRotated( d, pivot );

      //proj->up( up_rot );
      //proj->fwd( fwd_rot );
    //};

    //void tweak_Ry( float sign )
    //{
      //cml::OpticalDevice* proj = cml_data->cml->projector();

      //ofVec3f& pivot = proj->up();
      //float d = tweak_R_off * sign;

      //ofVec3f fwd_rot = proj->fwd().getRotated( d, pivot );
      //ofVec3f left_rot = proj->left().getRotated( d, pivot );

      //proj->left( left_rot );
      //proj->fwd( fwd_rot );
    //};

    void tweak_Tx( float sign )
    {
      cml::OpticalDevice* proj = cml_data->cml->projector();
      float d = tweak_T_off * sign;
      ofVec3f tt = proj->loc() + ofVec3f(d,0.,0.); 
      proj->loc( tt );
    };

    void tweak_Ty( float sign )
    {
      cml::OpticalDevice* proj = cml_data->cml->projector();
      float d = tweak_T_off * sign;
      ofVec3f tt = proj->loc() + ofVec3f(0.,d,0.); 
      proj->loc( tt );
    };

    void render_hue_tex()
    {
      if (cml_data == NULL) return;

      DepthComponent* depth_data = component<DepthComponent>("input");

      if ( depth_data->depth_ofpix_mm == NULL )
        return;

      int w = render_data->width;
      int h = render_data->height;

      ofSetColor(255);
      cml_data->cml->depth_camera()->get_hue_tex_ref( depth_data->depth_ofpix_mm->getPixels() ).draw( 0, 0, w, h );
    }; 

};

