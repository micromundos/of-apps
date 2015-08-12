#include "ofApp.h"


void ofApp::setup()
{
  ofSetDataPathRoot( __data_path__ );
  ofSetFrameRate(30);
  ofSetVerticalSync(false);
  ofSetLogLevel(OF_LOG_NOTICE);

  draw_rgb = false;

  w = 640;
  h = 480;

  rgb_cam.setDeviceID( 1 );
  rgb_cam.initGrabber( w, h, true );

  kinect.setRegistration(true);
  // ir, rgb, texture
  kinect.init(false, true, true);
	kinect.open();

	aruco.setupXML( "calib/intrinsics_rgb.aruco.yml", ofGetWidth(), ofGetHeight(), "", 0.07  );

  load_intrinsics( calib_kinect, int_k, "calib/intrinsics_kinect.aruco.yml" );
  load_intrinsics( calib_rgb, int_rgb, "calib/intrinsics_rgb.aruco.yml" );
  load_extrinsics("calib/extrinsics_rgb_to_kinect.yml"); //"extrinsics_kinect_to_rgb.yml"
}

void ofApp::update()
{
  ofSetWindowTitle(ofToString(ofGetFrameRate(),2));

	ofBackground(100);

	kinect.update();
  rgb_cam.update();

  if ( rgb_cam.isFrameNew() )
  {
    //copy
    ofPixels pix = rgb_cam.getPixelsRef();
    aruco.detectMarkers( pix );
  } 
}

void ofApp::draw()
{  

  if ( draw_rgb )
  {
    rgb_cam.draw( 0, 0 );
  }
  else 
  {
    //draw_kinect_undistorted( 0, 0 );
    kinect.draw( 0, 0, w, h );
  }

  aruco.draw2d();
  aruco.draw3d();

  vector<aruco::Marker>& markers = aruco.getMarkers();
  for ( int i = 0; i < markers.size(); i++ )
  {
    aruco::Marker& m = markers[i];

    cv::Mat mR(3,3,CV_32FC1);
    cv::Rodrigues( m.Rvec, mR ); 

    //ofMatrix4x4 mMV=aruco.getModelViewMatrix(i); 
    //ofMatrix4x4 mMV = get_marker_mv(m);
    //ofVec3f mT = mMV.getTranslation();

    ofVec3f mT;
    mT.x = m.Tvec.at<float>(0,0);
    mT.y = m.Tvec.at<float>(1,0);
    mT.z = m.Tvec.at<float>(2,0);

    //ofMatrix4x4 mk_MV = extrinsics.ofMV * mMV;
    //ofVec3f p3_k = mk_MV.getTranslation();

    //transform_to_depth
    //cv::Mat mTvec = m.Tvec.clone();
    //mTvec.at<float>(0,0) = mTvec.at<float>(0,0) - m.ssize;
    //mTvec.at<float>(1,0) = mTvec.at<float>(1,0) + m.ssize * 0.5;
    cv::Mat mk_T = extrinsics.R * m.Tvec + extrinsics.T; 
    ofVec3f p3_k;
    p3_k.x = mk_T.at<float>(0,0);
    p3_k.y = mk_T.at<float>(1,0);
    p3_k.z = mk_T.at<float>(2,0);

    ofVec2f p2_k; 
    project( calib_kinect.getDistortedIntrinsics(), p3_k, p2_k );
    //project( calib_kinect.getUndistortedIntrinsics(), p3_k, p2_k );

    // test projection back on rgb 
    ofVec2f p2_rgb; 
    project( calib_rgb.getDistortedIntrinsics(), mT, p2_rgb );
    //project( calib_rgb.getUndistortedIntrinsics(), mT, p2_rgb );


    cv::Point2f ctr = m.getCenter();

    ofLog() 
      << "marker= " << i << "\n"
      << "Tvec= " << m.Tvec << "\n"
      << "Rvec= " << m.Rvec << "\n"
      << "T= " << mT << "\n"
      //<< "MV= \n" << mMV << "\n"
      //<< "MV kinect= \n" << mk_MV << "\n"
      //<< "MV calib stereo= \n" << extrinsics.ofMV << "\n"
      //<< "mR= \n" << mR << "\n" 
      << "p3 kinect= " << p3_k << "\n"
      << "p2 kinect= " << p2_k << "\n"
      //<< " / marker ctr= " << ctr << "\n"
      << "\n";

    ofPushStyle();
    {
      ofSetColor(ofColor::magenta);
      ofRect( p2_k.x, p2_k.y, 5, 5 );

      ofSetColor(ofColor::cyan);
      ofRect( p2_rgb.x, p2_rgb.y, 5, 5 );
    }
    ofPopStyle();

    //aruco.begin(i);
    //drawMarker( 0.15, ofColor::red );
    //aruco.end();
  }

  draw_epilines();

  ofDrawBitmapStringHighlight(
      "axis: aruco detection",
      2, 12, 
      ofColor::yellow, ofColor::black);

  ofDrawBitmapStringHighlight(
      "cyan point: marker re-projected on rgb",
      2, 32, 
      ofColor::cyan, ofColor::black);

  ofDrawBitmapStringHighlight(
      "magenta point: marker re-projected on kinect",
      2, 52, 
      ofColor::magenta, ofColor::black);

  string curcam = draw_rgb?"rgb":"kinect";
  ofDrawBitmapStringHighlight(
      "camera: "+curcam
      +"  (press \'v\' to change)",
      2, ofGetHeight()-10, 
      ofColor::yellow, ofColor::black);
}

void ofApp::draw_epilines()
{
  //rgb corners epilines into kinect

  ofPushStyle();
  ofSetColor( ofColor::yellow );
  for ( int i = 0; i < epilines.size(); i++ )
  {
    //ax + bx + c = 0 
    cv::Vec3f& line = epilines[i];
    ofLine(
        ofVec2f( 0, -line[2]/line[1]),
        ofVec2f( w, -(line[2]+line[0]*w)/line[1])
        );
  }
  ofPopStyle();
};

void ofApp::project( const ofxCv::Intrinsics& intrinsics, const ofVec3f& p3, ofVec2f& p2 )
{
  cv::Mat cameraMatrix = intrinsics.getCameraMatrix();

  float fx = cameraMatrix.at<double>(0, 0);
  float fy = cameraMatrix.at<double>(1, 1);
  float cx = cameraMatrix.at<double>(0, 2);
  float cy = cameraMatrix.at<double>(1, 2);

  //cv::Point2d principalPoint = intrinsics.getPrincipalPoint();
  //float cx = principalPoint.x;
  //float cy = principalPoint.y;

  p2.x = (p3.x * fx / p3.z) + cx;
  p2.y = (p3.y * fy / p3.z) + cy;
};

void ofApp::draw_kinect_undistorted( int x, int y )
{
  //TODO 
  //dont recompute undistortion when !isFrameNew
  //if ( !kinect.isFrameNew() ) return;
  ofPixels pix_kinect = kinect.getPixelsRef();
  ofImage undistorted_kinect;
  ofxCv::imitate(undistorted_kinect, pix_kinect);
  calib_kinect.undistort( ofxCv::toCv(pix_kinect), ofxCv::toCv(undistorted_kinect) );
  undistorted_kinect.update();
  undistorted_kinect.draw( x, y );
};


void ofApp::load_intrinsics( ofxCv::Calibration& calib, ofxCv::Intrinsics& intrinsics, string filename )
{
  bool absolute = false;

  cv::FileStorage fs( ofToDataPath(filename, absolute), cv::FileStorage::READ );

  cv::Size imageSize, sensorSize;
  cv::Mat cameraMatrix;

  cv::Mat distCoeffs;
  float reprojectionError;

  fs["camera_matrix"] >> cameraMatrix;
  fs["image_width"] >> imageSize.width;
  fs["image_height"] >> imageSize.height;
  fs["sensor_width"] >> sensorSize.width;
  fs["sensor_height"] >> sensorSize.height;
  fs["distortion_coefficients"] >> distCoeffs;
  fs["reprojection_error"] >> reprojectionError;

  intrinsics.setup( cameraMatrix, imageSize, sensorSize ); 
  calib.setIntrinsics( intrinsics, distCoeffs ); //fires calib_xxx.updateUndistortion();
};

void ofApp::load_extrinsics(string filename)
{
  bool absolute = false;

  cv::FileStorage fs( ofToDataPath(filename, absolute), cv::FileStorage::READ );

  fs["R"] >> extrinsics.R;
  fs["T"] >> extrinsics.T;
  fs["E"] >> extrinsics.E;
  fs["F"] >> extrinsics.F;

  if ( extrinsics.R.type() != CV_32FC1 ) 
    extrinsics.R.convertTo( extrinsics.R, CV_32FC1 );

  if ( extrinsics.T.type() != CV_32FC1 ) 
    extrinsics.T.convertTo( extrinsics.T, CV_32FC1 );

  if ( extrinsics.E.type() != CV_32FC1 ) 
    extrinsics.E.convertTo( extrinsics.E, CV_32FC1 );

  if ( extrinsics.F.type() != CV_32FC1 ) 
    extrinsics.F.convertTo( extrinsics.F, CV_32FC1 );


  vector<cv::Point2f> corners;
  corners.push_back( cv::Point2f( 0,0 ) );
  corners.push_back( cv::Point2f( 0,w ) );
  corners.push_back( cv::Point2f( h,w ) );
  corners.push_back( cv::Point2f( h,0 ) );
  cv::computeCorrespondEpilines(
      cv::Mat(corners), 1,
      extrinsics.F,
      epilines );


  //@#$%ˆ& scale.....
  float scale = 1.;

  extrinsics.T *= scale;


  //ofQuaternion q;
  ////m.Rvec[a,b,c]
  //float theta = sqrt(a^2 + b^2 + c^2);
  //ofVec3f axis = [a/theta, b/theta, c/theta]; 
  //q.makeRotate( theta, axis );
  //extrinsics.MV.setRotate(q);


  //from Aruco CameraParameters::getCameraLocation
  extrinsics.MV = cv::Mat::eye(4,4,CV_32FC1);

  //rotation
  for (int i=0;i<3;i++)
    for (int j=0;j<3;j++)
      extrinsics.MV.at<float>(i,j) = extrinsics.R.at<float>(i,j);

  //now, add the translation
  for (int i=0;i<3;i++)
    extrinsics.MV.at<float>(i,3) = extrinsics.T.at<float>(i,0);

  //extrinsics.MV.inv(); 


  //from Aruco Marker::glGetModelViewMatrix

  //rotation
  float para[3][4];
  for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) 
      para[i][j] = extrinsics.R.at<float>(i,j);

  //now, add the translation
  para[0][3] = extrinsics.T.at<float>(0,0);
  para[1][3] = extrinsics.T.at<float>(1,0);
  para[2][3] = extrinsics.T.at<float>(2,0);

  // R1
  extrinsics.glMV[0 + 0*4] = para[0][0];
  extrinsics.glMV[0 + 1*4] = para[0][1];
  extrinsics.glMV[0 + 2*4] = para[0][2];
  extrinsics.glMV[0 + 3*4] = para[0][3];
  // R2
  extrinsics.glMV[1 + 0*4] = para[1][0];
  extrinsics.glMV[1 + 1*4] = para[1][1];
  extrinsics.glMV[1 + 2*4] = para[1][2];
  extrinsics.glMV[1 + 3*4] = para[1][3];
  // R3
  extrinsics.glMV[2 + 0*4] = -para[2][0];
  extrinsics.glMV[2 + 1*4] = -para[2][1];
  extrinsics.glMV[2 + 2*4] = -para[2][2];
  extrinsics.glMV[2 + 3*4] = -para[2][3];
  // R4
  extrinsics.glMV[3 + 0*4] = 0.0;
  extrinsics.glMV[3 + 1*4] = 0.0;
  extrinsics.glMV[3 + 2*4] = 0.0;
  extrinsics.glMV[3 + 3*4] = 1.0;

  if (scale != 0.0)
  {
    extrinsics.glMV[12] *= scale;
    extrinsics.glMV[13] *= scale;
    extrinsics.glMV[14] *= scale;
  }


  //copy to of mv
  //for (int i=0;i<16;i++)
    //extrinsics.ofMV.getptr()[i] = extrinsics.glMV[i];

  //copy to of MV
  for (int i=0;i<4;i++)
  for (int j=0;j<4;j++)
    extrinsics.ofMV(i,j) = extrinsics.MV.at<float>(i,j);


  extrinsics.ofMV = ofMatrix4x4::getTransposedOf( extrinsics.ofMV );
  //extrinsics.ofMV.scale( 1, 1, -1 );

  ofLog()
      << "\n extrinsics.T = \n" << extrinsics.T
      << "\n extrinsics.R = \n" << extrinsics.R
      << "\n extrinsics.MV = \n" << extrinsics.MV
      << "\n extrinsics.ofMV = \n" << extrinsics.ofMV
      << "\n";
};

ofMatrix4x4 ofApp::get_marker_mv(aruco::Marker& m)
{
  ofMatrix4x4 ofMV;

  cv::Mat mR(3,3,CV_32FC1);
  cv::Rodrigues( m.Rvec, mR );

  cv::Mat MV = cv::Mat::eye(4,4,CV_32FC1);

  //rotation
  for (int i=0;i<3;i++)
  for (int j=0;j<3;j++)
    MV.at<float>(i,j) = mR.at<float>(i,j);

  //now, add the translation
  for (int i=0;i<3;i++)
    MV.at<float>(i,3) = m.Tvec.at<float>(i,0);

  //MV.inv();


  float glMV[16];

  //rotation
  float para[3][4];
  for (int i=0;i<3;i++)
  for (int j=0;j<3;j++) 
    para[i][j] = mR.at<float>(i,j);

  //now, add the translation
  para[0][3] = m.Tvec.at<float>(0,0);
  para[1][3] = m.Tvec.at<float>(1,0);
  para[2][3] = m.Tvec.at<float>(2,0);

  // R1
  glMV[0 + 0*4] = para[0][0];
  glMV[0 + 1*4] = para[0][1];
  glMV[0 + 2*4] = para[0][2];
  glMV[0 + 3*4] = para[0][3];
  // R2
  glMV[1 + 0*4] = para[1][0];
  glMV[1 + 1*4] = para[1][1];
  glMV[1 + 2*4] = para[1][2];
  glMV[1 + 3*4] = para[1][3];
  // R3
  glMV[2 + 0*4] = -para[2][0];
  glMV[2 + 1*4] = -para[2][1];
  glMV[2 + 2*4] = -para[2][2];
  glMV[2 + 3*4] = -para[2][3];
  // R4
  glMV[3 + 0*4] = 0.0;
  glMV[3 + 1*4] = 0.0;
  glMV[3 + 2*4] = 0.0;
  glMV[3 + 3*4] = 1.0;


  //copy to of MV
  //for (int i=0;i<16;i++)
    //ofMV.getPtr()[i] = glMV[i];

  //copy to of MV
  for (int i=0;i<4;i++)
  for (int j=0;j<4;j++)
    ofMV(i,j) = MV.at<float>(i,j);

  ofMV = ofMatrix4x4::getTransposedOf( ofMV );
  //ofMV.scale( 1, 1, -1 );

  return ofMV;
};

void ofApp::drawMarker( float size, const ofColor& color )
{
  ofDrawAxis(size);
  ofPushMatrix();
  // move up from the center by size*.5
  // to draw a box centered at that point
  ofTranslate(0,size*0.5,0);
  ofFill();
  ofSetColor(color,50);
  ofBox(size);
  ofNoFill();
  ofSetColor(color);
  ofBox(size);
  ofPopMatrix();
}

void ofApp::keyPressed(int key)
{
  if ( key == 'v' ) 
  {
    draw_rgb = !draw_rgb;
  }
}

void ofApp::keyReleased(int key)
{

}


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
