#include "ofApp.h"

using namespace ofxCv;

const float diffThreshold = 6.; //2.5; // maximum amount of movement
const float timeThreshold = 1; // minimum time between snapshots
//const int startCleaning = 10; // start cleaning outliers after this many samples

void ofApp::setup() 
{
  ofSetFrameRate(30);
  ofSetVerticalSync(true);
  ofSetLogLevel(OF_LOG_NOTICE);

  capture = false;
  lastTime = 0;

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

  calib_settings = load_calib_settings();

  init_calib();
  allocate();
}

void ofApp::allocate()
{
  pix_kinect.allocate( w, h, chan );
  pix_ps3eye.allocate( w, h, chan );

  imitate(undistorted_kinect, pix_kinect);
	imitate(previous_kinect, pix_kinect);
	imitate(diff_kinect, pix_kinect);

  imitate(undistorted_ps3eye, pix_ps3eye);
	imitate(previous_ps3eye, pix_ps3eye);
	imitate(diff_ps3eye, pix_ps3eye);

  //undistorted.allocate( w, h, OF_IMAGE_COLOR );
  //previous.allocate( w, h, chan );
  //diff.allocate( w, h, chan );
}

void ofApp::update() 
{
  ofSetWindowTitle(ofToString(ofGetFrameRate(),2)); 

  curTime = ofGetElapsedTimef(); 

  bool _kinect = update_kinect();
  bool _ps3eye = update_ps3eye();

  if ( !_kinect || !_ps3eye )
    return;

  if ( !capture || curTime - lastTime < timeThreshold )
    return;

  if ( ! find_board( calib_kinect, camMat_kinect ) || ! find_board( calib_ps3eye, camMat_ps3eye ) )
  {
    ofLogWarning() << "did not found the chessboard on any camera";
    return;
  }

  update_calib( "kinect", calib_kinect, camMat_kinect, pix_kinect, undistorted_kinect );
  update_calib( "ps3eye", calib_ps3eye, camMat_ps3eye, pix_ps3eye, undistorted_ps3eye );

  lastTime = curTime;
}

void ofApp::draw() 
{
  ofBackground(100);
  ofSetColor(255); 

  kinect.draw( 0, 0, w, h );
  //kinect.drawDepth( 0, h, w, h );

  //ps3.draw( 0, w );
  ps3_tex.draw( w, 0 ); 

  drawHighlightString("kinect movement: " + ofToString(diffMean_kinect), 0, 20, cyanPrint);
  drawHighlightString("ps3eye movement: " + ofToString(diffMean_ps3eye), w, 20, cyanPrint);

  debug_calib(calib_kinect, "kinect", 0);
  debug_calib(calib_ps3eye, "ps3eye", w);

  render_calib(calib_kinect, 0);
  render_calib(calib_ps3eye, w);

  undistorted_kinect.draw( 0, h );
  undistorted_ps3eye.draw( w, h );
}

void ofApp::calib_save_aruco_format( ofxCv::Calibration& calibration, string name )
{
  if ( ! calibration.isReady() )
  {
    ofLog(OF_LOG_ERROR, "calib_save_aruco_format for " + name + " failed, because your calibration isn't ready yet!");
  }

  string filename = "calibration_aruco_"+ name +".yml";
  bool absolute = false;

  cv::FileStorage fs( ofToDataPath(filename, absolute), cv::FileStorage::WRITE);

  const ofxCv::Intrinsics& distorted_intrinsics = calibration.getDistortedIntrinsics();

  cv::Size imageSize = distorted_intrinsics.getImageSize();
  cv::Size sensorSize = distorted_intrinsics.getSensorSize();
  Mat cameraMatrix = distorted_intrinsics.getCameraMatrix();

  fs << "camera_matrix" << cameraMatrix;
  fs << "image_width" << imageSize.width;
  fs << "image_height" << imageSize.height;
  fs << "sensor_width" << sensorSize.width;
  fs << "sensor_height" << sensorSize.height;
  fs << "distortion_coefficients" << calibration.getDistCoeffs();
  fs << "reprojection_error" << calibration.getReprojectionError();

}

void ofApp::calib_save_stereo_RT( string src_name, ofxCv::Calibration& src_calib, string dst_name, ofxCv::Calibration& dst_calib )
{

  Mat R,T;

  if ( ! src_calib.getTransformation( dst_calib, R, T ) ) return;

  string filename = "calibration_RT_" + src_name + "_to_" + dst_name + ".yml";
  bool absolute = false;

  cv::FileStorage fs( ofToDataPath(filename, absolute), cv::FileStorage::WRITE); 

  fs << "R" << R;
  fs << "T" << T;
}

void ofApp::render_calib( ofxCv::Calibration& calibration, int x )
{
  if ( ! calibration.isReady() )
    return;
  ofPushMatrix();
  ofTranslate( x, 0, 0 );
  calibration.draw(calibration.size()-1);
  ofPopMatrix();
}

void ofApp::debug_calib( ofxCv::Calibration& calibration, string name, int x )
{ 
  drawHighlightString(name, x, 40, yellowPrint, ofColor(0));

  stringstream intrinsics;
  intrinsics << "fov: " << toOf(calibration.getDistortedIntrinsics().getFov()) << " distCoeffs: " << calibration.getDistCoeffs();

  drawHighlightString(intrinsics.str(), x, 60, yellowPrint, ofColor(0));

  drawHighlightString("reproj error: " + ofToString(calibration.getReprojectionError()) + " from " + ofToString(calibration.size()), x, 80, magentaPrint);

  for ( int i = 0; i < calibration.size(); i++ )
  {
    drawHighlightString(ofToString(i) + ": " + ofToString(calibration.getReprojectionError(i)), x, 100 + 16 * i, magentaPrint);
  }
}

void ofApp::keyPressed(int key) 
{
  if ( key == ' ' ) 
  {
    capture = !capture;
  }

  else if ( key == 's' )
  {
    calib_save_stereo_RT( "ps3eye", calib_ps3eye, "kinect", calib_kinect );
    calib_save_stereo_RT( "kinect", calib_kinect, "ps3eye", calib_ps3eye );
  }

}

bool ofApp::update_kinect()
{
  kinect.update(); 

  if ( kinect.isFrameNew() )
  {
    //copy
    pix_kinect = kinect.getPixelsRef();
  }

  return update_camMat( camMat_kinect, pix_kinect, previous_kinect, diff_kinect, &diffMean_kinect );
}

bool ofApp::update_ps3eye()
{
  ps3.update();

  if ( ps3.isFrameNew() )
  {
    ps3_tex.loadData(ps3.getPixelsRef());
    //copy
    pix_ps3eye = ps3.getPixelsRef();
    pix_ps3eye.setNumChannels( chan );
  }

  return update_camMat( camMat_ps3eye, pix_ps3eye, previous_ps3eye, diff_ps3eye, &diffMean_ps3eye );
}

bool ofApp::update_camMat( Mat& camMat, ofPixels& pix, ofPixels& previous, ofPixels& diff, float* diffMean )
{ 
  camMat = toCv(pix);
  Mat prevMat = toCv(previous);
  Mat diffMat = toCv(diff);

  //cout << "camMat " << camMat.cols << " x " << camMat.rows << endl;

  ofxCv::absdiff(prevMat,camMat,diffMat);	
  camMat.copyTo(prevMat);

  *diffMean = ofxCv::mean( Mat( ofxCv::mean(diffMat) ) )[0];

  return *diffMean < diffThreshold;
}

bool ofApp::find_board( ofxCv::Calibration& calibration, Mat& camMat )
{
  vector<Point2f> pointBuf;
  return calibration.findBoard(camMat, pointBuf);
}

bool ofApp::update_calib( string name, ofxCv::Calibration& calibration, Mat& camMat, ofPixels& pix, ofImage& undistorted )
{ 

  if ( !calibration.add( camMat ) )
  {
    ofLogError() << "update calib: this should not happen, we already found the chessboard !@#$%ˆ&*";
    return false;
  }

  cout << "re-calibrating " << name << endl;

  calibration.calibrate();

  //if ( calibration.size() > startCleaning ) 
  //{
    //calibration.clean();
  //}

  //calibration.save("calibration_" + name + ".yml");

  calib_save_aruco_format( calibration, name );

  if ( calibration.size() > 0 ) 
  {
    calibration.undistort( toCv(pix), toCv(undistorted) );
    undistorted.update();
  }
}

cv::FileStorage ofApp::load_calib_settings()
{
  cv::FileStorage calib_settings(ofToDataPath("settings.yml"), cv::FileStorage::READ);
  if ( ! calib_settings.isOpened() )
    throw "calib setting not loaded";
  return calib_settings;
}

void ofApp::init_calib()
{
  int xCount = calib_settings["xCount"];
  int yCount = calib_settings["yCount"];
  float squareSize = calib_settings["squareSize"]; 

  ofxCv::CalibrationPattern patternType;
  switch( calib_settings["patternType"] ) 
  {
    case 0: 
      patternType = CHESSBOARD; 
      break;
    case 1: 
      patternType = CIRCLES_GRID; 
      break;
    case 2: 
      patternType = ASYMMETRIC_CIRCLES_GRID; 
      break;
  }

  calib_kinect.setPatternSize(xCount, yCount);
  calib_kinect.setSquareSize(squareSize);
  calib_kinect.setPatternType(patternType);

  calib_ps3eye.setPatternSize(xCount, yCount);
  calib_ps3eye.setSquareSize(squareSize);
  calib_ps3eye.setPatternType(patternType);

}

