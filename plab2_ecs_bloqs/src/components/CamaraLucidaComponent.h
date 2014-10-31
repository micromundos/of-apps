#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxCamaraLucida.h"
#include "keys.h"

class CamaraLucidaComponent : public artemis::Component 
{
  public:
    string id;
    cml::CamaraLucida* cml;

    CamaraLucidaComponent( string id )
    {
      this->id = id; 

      cml::Config cfg;

      cfg.key_debug = keys::cml_key_debug;
      cfg.key_help = keys::cml_key_help;
      cfg.key_scene_zoom = keys::cml_key_scene_zoom;
      cfg.key_scene_reset = keys::cml_key_scene_reset;
      cfg.key_view_prev = keys::cml_key_view_prev;
      cfg.key_view_next = keys::cml_key_view_next;

      cfg.tex_width = 1024 * 2;
      cfg.tex_height = 768 * 2;

      cml = new cml::CamaraLucida(cfg);
    };
};

