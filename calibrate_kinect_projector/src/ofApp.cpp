#include "ofApp.h"


void ofApp::setup()
{
  ofSetFrameRate(30);
  ofSetVerticalSync(true);
  ofSetLogLevel(OF_LOG_NOTICE);

  w = 640;
  h = 480;
  chan = 3;

  kinect.setRegistration(true);
  // ir, rgb, texture
  kinect.init(true, true, true);
  kinect.open();
  kinect.update(); 

  pix_kinect_rgb = kinect.getPixelsRef(); //copy

  calibration.init( pix_kinect_rgb, "calib_kinect.yml", "kinect_rgb", "proj_lg" );
}


void ofApp::exit() 
{
  kinect.close();
}


void ofApp::update()
{
  ofSetWindowTitle(ofToString(ofGetFrameRate(),2));

  kinect.update();

  if ( !kinect.isFrameNew() ) return;

  pix_kinect_rgb = kinect.getPixelsRef(); //copy

  calibration.update( pix_kinect_rgb );
}


void ofApp::draw()
{
  ofBackground(100);
  ofSetColor(255); 

  kinect.draw( 0, 0, w, h );
  //kinect.drawDepth( 0, h, w, h );

  calibration.render();
}


void ofApp::keyPressed(int key)
{
  if ( key == ' ' ) 
  {
    calibration.toggle_capture();
  }
}


void ofApp::keyReleased(int key)
{
  if ( key == ' ' ) 
  {
    calibration.toggle_capture();
  }

  else if ( key == 'c' )
  {
    calibration.calibrate();
  }

  else if ( key == 's' )
  {
    calibration.save_all();
  }

  else if ( key == 'r' )
  {
    calibration.reset();
  }
}


void ofApp::mouseMoved(int x, int y )
{

}


void ofApp::mouseDragged(int x, int y, int button)
{

}


void ofApp::mousePressed(int x, int y, int button)
{

}


void ofApp::mouseReleased(int x, int y, int button)
{

}


void ofApp::windowResized(int w, int h)
{

}


void ofApp::gotMessage(ofMessage msg)
{

}


void ofApp::dragEvent(ofDragInfo dragInfo)
{ 

}

