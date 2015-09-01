#include "ofApp.h"

using namespace ofxCv;

const float diffThreshold = 4; // maximum amount of movement
const float timeThreshold = 3; // minimum time between snapshots
const int startCleaning = 10; // start cleaning outliers after this many samples
const float min_reprojection_error = 1.f;

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

  calib_cml.diffThreshold = diffThreshold;

  kinect.setRegistration(true);
  // ir, rgb, texture
  kinect.init(false, true, true);
  kinect.open();
  kinect.update(); 

  w = kinect.getWidth();
  h = kinect.getHeight();

  load_pattern_settings( settings["params"]["calib_kinect_rgb_mono"]["cam_pattern_path"].asString() );

  pix_kinect_rgb = kinect.getPixelsRef(); //copy

  ofxCv::imitate(undistorted, pix_kinect_rgb);
  ofxCv::imitate(previous, pix_kinect_rgb);
  ofxCv::imitate(diff, pix_kinect_rgb);

  lastTime = 0;
  active = false;
}

void ofApp::exit() 
{
  kinect.close();
}

void ofApp::update() 
{
  ofSetWindowTitle(ofToString(ofGetFrameRate(),2));

  kinect.update(); 
  if ( !kinect.isFrameNew() ) 
    return;

  pix_kinect_rgb = kinect.getPixelsRef(); //copy

  cv::Mat camMat;
  bool updated = calib_cml.update_cam( camMat, pix_kinect_rgb, previous, diff, &diffMean );

  float curTime = ofGetElapsedTimef();
  if ( active && curTime - lastTime > timeThreshold && updated )
  {
    if ( calibration.add(camMat) ) 
    {
      calibration.calibrate();
      if ( calibration.size() > startCleaning )
      {
        calibration.clean(min_reprojection_error);
      }

      //let users save
      //save_all();

      lastTime = curTime;
    }
  }

  calib_cml.undistort( calibration, pix_kinect_rgb, undistorted );
}

void ofApp::draw() 
{
  ofSetColor(255);

  kinect.draw( 0, 0, w, h );
  undistorted.draw(w, 0);

  stringstream intrinsics;
  intrinsics << "fov: " << toOf(calibration.getDistortedIntrinsics().getFov()) << " distCoeffs: " << calibration.getDistCoeffs();
  drawHighlightString(intrinsics.str(), 10, 20, yellowPrint, ofColor(0));

  drawHighlightString("movement: " + ofToString(diffMean), 10, 40, cyanPrint);

  drawHighlightString("reproj error: " + ofToString(calibration.getReprojectionError()) + " from " + ofToString(calibration.size()), 10, 60, magentaPrint);

  for ( int i = 0; i < calibration.size(); i++ ) 
  {
    drawHighlightString(ofToString(i) + ": " + ofToString(calibration.getReprojectionError(i)), 10, 80 + 16 * i, magentaPrint);
  }

  ofDrawBitmapStringHighlight(
      " toggle capture: spacebar \n save: \'s\' \n reset: \'r\' \n", 
      ofGetWidth()-220,ofGetHeight()-40, 
      ofColor::yellow, ofColor::black);
}

void ofApp::keyPressed(int key) 
{
  if (key == ' ') 
  {
    active = !active;
  }

  if (key == 's') 
  {
    save_all();
  }

  if (key == 'r') 
  {
    calibration.reset();
  }
}

void ofApp::keyReleased(int key) 
{
  //if (key == ' ') 
  //{
    //active = !active;
  //}
}

void ofApp::save_all()
{
  string name = "kinect";
  string folder = "calib";
  calib_cml.save_intrinsics( calibration, name, folder, "ofxcv" );
  calib_cml.save_intrinsics( calibration, name, folder, "aruco" );
}

bool ofApp::load_pattern_settings( string pattern_settings_file )
{
  cv::FileStorage settings( ofToDataPath(pattern_settings_file), cv::FileStorage::READ );

  if ( !settings.isOpened() )
  {
    ofLogFatalError() << "could not open projector pattern settings file: " << ofToDataPath(pattern_settings_file);
    ofExit();
    return false;
  }

  int xCount = settings["pattern_width"];
  int yCount = settings["pattern_height"];
  float squareSize = settings["pattern_square_size"];

  calibration.setPatternSize(xCount, yCount);
  calibration.setSquareSize(squareSize);
  calibration.setPatternType(CHESSBOARD);
};

