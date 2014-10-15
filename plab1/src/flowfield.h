#pragma once

#include "ofMain.h"
#include "ofVectorMath.h"
#include "ofxFx.h"
#include "params.h"
#include "coord.h"

class FlowField : public Parametrizable
{
public:

	FlowField();
	~FlowField();

	void init(
      Params* params,
      coord ff2depth,
      coord tex2ff,
      coord tex2depth );

	void update( 
      uint16_t *mm_depth_pix );

  void update(
    uint16_t *mm_depth_pix,
    ofTexture& depth_ftex );

  void render();
  
  const ofVec2f& get_force( ofVec2f texloc );

private:

  ofVec2f* field;
  //ofxFXObject ffgpu;
  //ofFloatPixels ffpix;

  coord ff2depth;
  coord tex2ff;
  coord tex2depth;

  int width, height;

  uint16_t get_depth( 
    int idx, 
    uint16_t *depth_pix );

};

