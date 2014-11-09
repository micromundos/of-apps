#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ecs/ECScomponent.h"
#include "ofxCamaraLucida.h"
#include "keys.h"

class CamaraLucidaComponent : public ECScomponent 
{
  public:

    cml::CamaraLucida* cml;

    CamaraLucidaComponent(string _id) : ECScomponent(_id) 
    { cml = NULL; };

    virtual void init( Json::Value d, ofParameterGroup* p )
    {
      ECScomponent::init(d,p);
      init_cml( d.get( "tex_width", 1024 ).asInt(), d.get( "tex_height", 768).asInt(), d.get( "mesh_res", 2).asInt() );
    };

  private:
    
    void init_cml( int tex_width, int tex_height, int mesh_res )
    {
      if ( cml != NULL )
      {
        ofLogWarning("CamaraLucidaComponent") << "init_cml: cml is already inited";
        return;
      }

      cml::Config cfg;

      cfg.key_debug = keys::cml_key_debug;
      cfg.key_help = keys::cml_key_help;
      cfg.key_scene_zoom = keys::cml_key_scene_zoom;
      cfg.key_scene_reset = keys::cml_key_scene_reset;
      cfg.key_view_prev = keys::cml_key_view_prev;
      cfg.key_view_next = keys::cml_key_view_next;

      cfg.tex_width = tex_width;
      cfg.tex_height = tex_height;
      cfg.mesh_res = mesh_res;

      cml = new cml::CamaraLucida( cfg );
    };
};

