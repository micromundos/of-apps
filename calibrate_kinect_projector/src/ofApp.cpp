#include "ofApp.h"

int yoff = 20; //window undecorated yoffset

void ofApp::setup()
{
  ofSetDataPathRoot( __data_path__ );
  ofSetFrameRate(30);
  ofSetVerticalSync(true);
  ofSetLogLevel(OF_LOG_NOTICE);
  ofSetWindowPosition( ofGetScreenWidth() - ofGetWidth(), 0 );
  //ofSetWindowPosition( 0, 0 );

  w = 640;
  h = 480;

  kinect.setRegistration(true);
  // ir, rgb, texture
  kinect.init(false, true, true);
  kinect.open();
  kinect.update(); 

  pix_kinect_rgb = kinect.getPixelsRef(); //copy
  calibration.init( pix_kinect_rgb, "calib/calib_kinect.ofxcv.yml", "kinect_rgb", "proj_lg" );

  capture_btn.addListener(this,&ofApp::capture);
  calibrate_btn.addListener(this,&ofApp::calibrate);
  save_calib_btn.addListener(this,&ofApp::save_calib);
  reset_calib_btn.addListener(this,&ofApp::reset_calib);

  string gui_settings = "calib/kinect_projector_gui.xml";
	gui.setup( "", gui_settings );
  gui.add( chessboard_brightness.setup( "chessboard_brightness", 127, 0, 255 ) );
  gui.add( chessboard_projected.setup( "chessboard_projected", true ) );
  gui.add( capture_btn.setup("capture (spacebar)") );
  gui.add( calibrate_btn.setup("calibrate (c)") );
  gui.add( save_calib_btn.setup("save calibration (s)") );
  gui.add( reset_calib_btn.setup("reset calibration (r)") );

  gui.setPosition( 
    w - gui.getWidth() - 10, 
    ofGetScreenHeight() - gui.getHeight() - 10 
  );
  gui.loadFromFile( gui_settings );
}


void ofApp::exit() 
{
  capture_btn.removeListener(this,&ofApp::capture);
  calibrate_btn.removeListener(this,&ofApp::calibrate);
  save_calib_btn.removeListener(this,&ofApp::save_calib);
  reset_calib_btn.removeListener(this,&ofApp::reset_calib);
  kinect.close();
}


void ofApp::capture()
{
  calibration.toggle_capture();
}

void ofApp::calibrate()
{
  calibration.calibrate();
}

void ofApp::save_calib()
{
  calibration.save_all( "calib" );
}

void ofApp::reset_calib()
{
  calibration.reset();
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

  kinect.draw( 0, yoff, w, h );
  //kinect.drawDepth( 0, yoff+h, w, h );

  calibration.render( 0, yoff );
  calibration.render_chessboard( chessboard_projected ? ofGetScreenWidth() : 0, 0, chessboard_brightness );

  gui.draw();
}


void ofApp::keyPressed(int key)
{
  //if ( key == ' ' ) 
  //{
    //capture();
  //}
}


void ofApp::keyReleased(int key)
{
  if ( key == ' ' ) 
  {
    capture();
  }

  else if ( key == 'c' )
  {
    calibrate();
  }

  else if ( key == 's' )
  {
    save_calib();
  }

  else if ( key == 'r' )
  {
    reset_calib();
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
