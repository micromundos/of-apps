#pragma once

#include "params.h"
#include "ofxOpenCv.h"

namespace DepthProcess
{

class blobs
{
public: 

  void init(
      int depth_width, int depth_height );

  void update( 
      ofxCvGrayscaleImage img );

	void draw( 
      int x, int y, int w, int h );

  vector<ofxCvBlob>& list()
  {
    return blobfinder.blobs;
  };

private:

  int depth_width, depth_height;

	ofxCvContourFinder blobfinder;

};

}; //namespace

