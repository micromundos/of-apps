#pragma once

#include "ofxTimeMeasurements.h"

namespace keys
{
  //const char fullscreen = 'f';
  const char projector = 'p';

  //app
  const char cml_gpu = 'g';
  const char cml_wireframe = 'w';

  //cml tweaks
  const char cml_tweak_reset = 'q';
  const char cml_tweak_frustum = 'f';
  //const char cml_tweak_Rxy = 'e';
  const char cml_tweak_Rz = 'r';
  const char cml_tweak_Txy = 't';
  const char cml_tweak_save = 's';
  const char cml_tweak_load = 'l';

  //cml
  const char cml_key_debug = 'd';
  const char cml_key_help = 'h';
  const char cml_key_scene_zoom = 'z';
  const char cml_key_scene_reset = 'x';
  const char cml_key_view_next = 'v';

  //time measurements
  const char tm_toggle = '1'; //'Y';
  const char tm_edit = '2'; //'T';
  //const char tm_avg = '3'; //'A';

  void init()
  {
    ofxTimeMeasurements::instance()->setUIActivationKey( tm_edit );
  };

};

