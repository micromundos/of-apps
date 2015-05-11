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

  w = 640;
  h = 480;
  chan = 3;

  ps3.initGrabber(w, h);
  ps3.setAutogain(true);
  ps3.setAutoWhiteBalance(true);
  //ps3.setGain(uint8_t val);
  //ps3.setExposure(uint8_t val);
  //ps3.setSharpness(uint8_t val);
  //ps3.setContrast(uint8_t val);
  //ps3.setBrightness(uint8_t val);
  //ps3.setHue(uint8_t val);
  //ps3.setRedBalance(uint8_t val);
  //ps3.setBlueBalance(uint8_t val);

  kinect.setRegistration(true);
  // ir, rgb, texture
  kinect.init(false, true, true);
  kinect.open();

  kinect.update(); 
  ps3.update();

  pix_kinect_rgb = kinect.getPixelsRef(); //copy
  pix_ps3eye = ps3.getPixelsRef(); //copy
  pix_ps3eye.setNumChannels( chan );

  calibration.init( 
      settings["params"]["calib_kinect_p3eye"]["cam_pattern_path"].asString(),
      "kinect", pix_kinect_rgb, 
      "ps3eye", pix_ps3eye, 
      //load calibrated kinect intrinsics
      settings["params"]["calib_kinect_p3eye"]["calib_kinect_path"].asString() );
}


void ofApp::exit() 
{
  kinect.close();
  ps3.close();
}


void ofApp::update()
{
  ofSetWindowTitle(ofToString(ofGetFrameRate(),2));

  kinect.update(); 
  ps3.update();

  if ( !kinect.isFrameNew() && !ps3.isFrameNew() ) return;

  if ( kinect.isFrameNew() )
    pix_kinect_rgb = kinect.getPixelsRef(); //copy

  if ( ps3.isFrameNew() )
  {
    ps3_tex.loadData(ps3.getPixelsRef());
    pix_ps3eye = ps3.getPixelsRef(); //copy
    pix_ps3eye.setNumChannels( chan );
  }

  calibration.update( pix_kinect_rgb, pix_ps3eye );
}


void ofApp::draw()
{
  ofBackground(100);
  ofSetColor(255); 

  kinect.draw( 0, 0, w, h );
  //kinect.drawDepth( 0, h, w, h );

  //ps3.draw( 0, w );
  ps3_tex.draw( w, 0 );

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
    //save ps3 intrinsics & stereo
    //DONT save kinect intrinsics
    string folder = "calib";

    calibration.save_intrinsics( "ps3eye", folder, "ofxcv" );
    calibration.save_intrinsics( "ps3eye", folder, "aruco" );

    calibration.save_extrinsics( "kinect", "ps3eye", folder );
    calibration.save_extrinsics( "ps3eye", "kinect", folder );

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

