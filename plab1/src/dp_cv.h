#pragma once

#include "params.h"
#include "coord.h"
#include "ofxOpenCv.h"

namespace DepthProcess
{

class cv : public Parametrizable 
{
public: 

  void init(
      Params* params,
      coord cv2depth );

  void update( 
      uint16_t *mm_depth_pix );

	void draw( 
      int x, int y, int w, int h );

  ofxCvGrayscaleImage& image()
  {
    return img;
  };

private:

  coord cv2depth;

	ofxCvGrayscaleImage img;

  uint16_t get_depth( 
    int dx, int dy, 
    uint16_t *depth_pix );
  //uint16_t get_depth( 
    //int idx, uint16_t *depth_pix );

};

}; //namespace

