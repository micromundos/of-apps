#include "ofApp.h"


void ofApp::setup()
{
  ofSetDataPathRoot( __data_path__ );
  ofSetFrameRate(30);
  ofSetVerticalSync(true);
  ofSetLogLevel(OF_LOG_NOTICE);

  if ( !settings.open( "config/settings.json" ) ) 
  {
    ofLogFatalError() << "error opening settings.json";
    ofExit();
    return;
  }

  rgb_cam_name = settings["params"]["calib_kinect_rgb_stereo"]["rgb_cam_name"].asString();
  rgb_device_id = settings["params"]["calib_kinect_rgb_stereo"]["rgb_device_id"].asInt();
  rgb_width = settings["params"]["calib_kinect_rgb_stereo"]["rgb_width"].asInt();
  rgb_height = settings["params"]["calib_kinect_rgb_stereo"]["rgb_height"].asInt();
  rgb_width_draw = rgb_width / 2;
  rgb_height_draw = rgb_height / 2;

  ofLog() << "rgb size " << rgb_width << " x " << rgb_height;

  rgb_cam.setDeviceID( rgb_device_id );
  rgb_cam.initGrabber( rgb_width, rgb_height, true );

  kinect.setRegistration(true);
  // ir, rgb, texture
  kinect.init(false, true, true);
  kinect.open();

  kinect.update(); 
  rgb_cam.update();

  pix_kinect_rgb = kinect.getPixelsRef(); //copy
  pix_rgb = rgb_cam.getPixelsRef(); //copy

  calibration.init( 
      settings["params"]["calib_kinect_rgb_stereo"]["cam_pattern_path"].asString(),
      "kinect", pix_kinect_rgb, 
      rgb_cam_name, pix_rgb, 
      //load calibrated kinect intrinsics
      settings["params"]["calib_kinect_rgb_stereo"]["calib_kinect_path"].asString() );

  //window setup
  ofSetWindowShape( 
      kinect.width + rgb_width_draw, 
      rgb_height > kinect.height ? rgb_height : kinect.height );
  ofSetWindowPosition( 0, 0 );
}


void ofApp::exit() 
{
  kinect.close();
  rgb_cam.close();
}


void ofApp::update()
{
  ofSetWindowTitle(ofToString(ofGetFrameRate(),2));

  kinect.update(); 
  rgb_cam.update();

  if ( !kinect.isFrameNew() && !rgb_cam.isFrameNew() ) return;

  if ( kinect.isFrameNew() )
    pix_kinect_rgb = kinect.getPixelsRef(); //copy

  if ( rgb_cam.isFrameNew() )
  {
    pix_rgb = rgb_cam.getPixelsRef(); //copy
  }

  calibration.update( pix_kinect_rgb, pix_rgb );
}


void ofApp::draw()
{
  ofBackground(100);
  ofSetColor(255); 

  kinect.draw( 0, 0 );
  //kinect.drawDepth( 0, h );

  rgb_cam.draw( kinect.width, 0, rgb_width_draw, rgb_height_draw );

  calibration.render();

  ofDrawBitmapStringHighlight(
      " capture: spacebar \n save: \'s\' \n reset: \'r\' \n remove last: \'b\' \n", 
      0, ofGetHeight()-50, 
      ofColor::yellow, ofColor::black);
}


void ofApp::keyPressed(int key)
{
  if ( key == ' ' ) 
  {
    calibration.toggle_capture(); 
  }
  else if ( key == 'b' )
  {
    calibration.removeLast();
  }
}


void ofApp::keyReleased(int key)
{
  if ( key == ' ' ) 
  {
    calibration.toggle_capture();
  }

  else if ( key == 's' )
  {
    //save rgb intrinsics & stereo
    //DONT save kinect intrinsics
    string folder = "calib";

    calibration.save_intrinsics( rgb_cam_name, folder, "ofxcv" );
    calibration.save_intrinsics( rgb_cam_name, folder, "aruco" );

    calibration.save_extrinsics( "kinect", rgb_cam_name, folder );
    calibration.save_extrinsics( rgb_cam_name, "kinect", folder );

    //calibration.save_all( "calib" );
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

