#pragma once

#include "params.h"
#include "ofxFx.h"

namespace DepthProcess
{

class gpu : public Parametrizable
{
public: 

  void init(
      Params* params,
      int depth_width, 
      int depth_height );

  void update( 
      uint16_t *mm_depth_pix,
      ofTexture& depth_ftex );

	void draw( 
      int x, int y, int w, int h );

private:

  int depth_width, depth_height;

  //ofxFXObject flowfield;
  //ofxFXObject p3d;
  //ofxFXObject normals;
  //ofxFXObject normals_vis;
  //ofxFXObject bilateral;

};

}; //namespace

