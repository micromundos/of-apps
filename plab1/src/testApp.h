#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxCamaraLucida.h"
#include "cmlDepthmap_freenect.h"
#include "params.h"
#include "coord.h"
#include "traer/particles.h"
#include "flowfield.h"
#include "dp_cv.h"
#include "dp_gpu.h"
#include "dp_blobs.h"

class testApp : public ofBaseApp 
{
public:

  void setup();
  void update();
  void draw();
  void exit();

  void keyPressed(int key);
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void windowResized(int w, int h);

  ofxKinect kinect;
  float _near_mm;
  float _far_mm;

  cml::CamaraLucida *cml;
  cml::Depthmap_freenect *depthmap;
  void render_texture(ofEventArgs &args);
  void render_3d(ofEventArgs &args);
  void render_2d(ofEventArgs &args);

  Params params;
  Particles ps; 
  FlowField ff;
  DepthProcess::gpu gpu;
  DepthProcess::cv cv;
  DepthProcess::blobs blobs;

  void update_window( bool init = false );

};

