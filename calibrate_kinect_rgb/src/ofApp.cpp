#include "ofApp.h"

using namespace ofxCv;

const float timeThreshold = 1; // minimum time between snapshots
const int startCleaning = 10; // start cleaning outliers after this many samples


void ofApp::setup() 
{
  ofSetDataPathRoot( __data_path__ );
  ofSetFrameRate(30);
  ofSetVerticalSync(true);
  ofSetLogLevel(OF_LOG_NOTICE);

  kinect.setRegistration(true);
  // ir, rgb, texture
  kinect.init(false, true, true);
  kinect.open();
  kinect.update(); 

  w = kinect.getWidth();
  h = kinect.getHeight();

  load_pattern_settings("calib/cam_pattern.yml");

  pix_kinect_rgb = kinect.getPixelsRef(); //copy

  ofxCv::imitate(undistorted, pix_kinect_rgb);
  ofxCv::imitate(previous, pix_kinect_rgb);
  ofxCv::imitate(diff, pix_kinect_rgb);

  lastTime = 0;

  active = true;
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

  return;

  pix_kinect_rgb = kinect.getPixelsRef(); //copy

  cv::Mat camMat;
  bool updated = calibration_cml.update_cam( camMat, pix_kinect_rgb, previous, diff, &diffMean );

  //cv::Mat camMat = toCv(pix_kinect_rgb);
  //cv::Mat prevMat = toCv(previous);
  //cv::Mat diffMat = toCv(diff);
  //ofxCv::absdiff(prevMat, camMat, diffMat);	
  //camMat.copyTo(prevMat);
  //diffMean = ofxCv::mean( cv::Mat( ofxCv::mean(diffMat) ) )[0];

  float curTime = ofGetElapsedTimef();
  if ( active && curTime - lastTime > timeThreshold && updated )
  //if ( active && curTime - lastTime > timeThreshold && diffMean < diffThreshold )
  {
    if ( calibration.add(camMat) ) 
    {
      ofLog() << "re-calibrating";
      calibration.calibrate();
      if ( calibration.size() > startCleaning )
      {
        calibration.clean();
      }

      save_all();

      lastTime = curTime;
    }
  }

  if ( calibration.size() > 0 )
  {
    calibration.undistort(toCv(pix_kinect_rgb), toCv(undistorted));
    undistorted.update();
  }
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
}

void ofApp::keyPressed(int key) 
{
  if (key == ' ') 
  {
    active = !active;
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

  calibration_cml.save_intrinsics( calibration, name, folder, "ofxcv" );

  calibration_cml.save_intrinsics( calibration, name, folder, "aruco" );
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

