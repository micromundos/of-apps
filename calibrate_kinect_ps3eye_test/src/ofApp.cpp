#include "ofApp.h"


void ofApp::setup()
{
  ofSetDataPathRoot( __data_path__ );
  ofSetFrameRate(30);
  ofSetVerticalSync(true);
  ofSetLogLevel(OF_LOG_NOTICE);

  draw_ps3eye = true;

  w = 640;
  h = 480;

  ps3.initGrabber(w, h);
  ps3.setAutogain(true);
  ps3.setAutoWhiteBalance(true);

  kinect.setRegistration(true);
  // ir, rgb, texture
  kinect.init(false, true, true);
	kinect.open();

	aruco.setupXML( "calib/intrinsics_ps3eye.aruco.yml", ofGetWidth(), ofGetHeight() );

  load_aruco_kinect_calib("calib/intrinsics_kinect.aruco.yml");
  load_aruco_ps3_calib("calib/intrinsics_ps3eye.aruco.yml");
  load_stereo_calib("calib/extrinsics_ps3eye_to_kinect.yml"); //"extrinsics_kinect_to_ps3eye.yml"
}

void ofApp::update()
{
  ofSetWindowTitle(ofToString(ofGetFrameRate(),2));

	ofBackground(100);

	kinect.update();
  ps3.update();
  ps3_tex.loadData( ps3.getPixelsRef() );

  if ( ps3.isFrameNew() )
  {
    //copy
    ofPixels pix = ps3.getPixelsRef();
    pix.setNumChannels(3);
    aruco.detectMarkers( pix );
  } 
}

void ofApp::draw()
{  

  if ( draw_ps3eye )
  {
    ps3_tex.draw( 0, 0 );
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

    //ofMatrix4x4 mk_MV = calib_stereo.ofMV * mMV;
    //ofVec3f p3_k = mk_MV.getTranslation();

    cv::Mat mk_T = calib_stereo.R * m.Tvec + calib_stereo.T; 
    ofVec3f p3_k;
    p3_k.x = mk_T.at<float>(0,0) - m.ssize * 0.5;
    p3_k.y = mk_T.at<float>(1,0) - m.ssize * 0.5;
    p3_k.z = mk_T.at<float>(2,0);

    ofVec2f p2_k; 
    project_on_kinect( p3_k, p2_k ); 

    // test projection back on ps3eye 
    ofVec2f p2_ps3eye; 
    project_on_ps3( mT, p2_ps3eye );


    cv::Point2f ctr = m.getCenter();

    ofLog() 
      << "marker= " << i << "\n"
      << "Tvec= " << m.Tvec << "\n"
      << "Rvec= " << m.Rvec << "\n"
      << "T= " << mT << "\n"
      //<< "MV= \n" << mMV << "\n"
      //<< "MV kinect= \n" << mk_MV << "\n"
      //<< "MV calib stereo= \n" << calib_stereo.ofMV << "\n"
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
      ofRect( p2_ps3eye.x, p2_ps3eye.y, 5, 5 );
    }
    ofPopStyle();

    //aruco.begin(i);
    //drawMarker( 0.15, ofColor::red );
    //aruco.end();
  }

  ofDrawBitmapStringHighlight(
      "axis: aruco detection",
      2, 12, 
      ofColor::yellow, ofColor::black);

  ofDrawBitmapStringHighlight(
      "cyan point: marker re-projected on ps3eye",
      2, 32, 
      ofColor::cyan, ofColor::black);

  ofDrawBitmapStringHighlight(
      "magenta point: marker re-projected on kinect",
      2, 52, 
      ofColor::magenta, ofColor::black);

  string curcam = draw_ps3eye?"ps3eye":"kinect";
  ofDrawBitmapStringHighlight(
      "camera: "+curcam
      +"  (press \'v\' to change)",
      2, ofGetHeight()-10, 
      ofColor::yellow, ofColor::black);
}

void ofApp::project_on_kinect( const ofVec3f& p3, ofVec2f& p2 )
{
  const ofxCv::Intrinsics& _int = calib_kinect.getDistortedIntrinsics();
  //const ofxCv::Intrinsics& _int = calib_kinect.getUndistortedIntrinsics();

  cv::Mat cameraMatrix = _int.getCameraMatrix();

  float fx = cameraMatrix.at<double>(0, 0);
  float fy = cameraMatrix.at<double>(1, 1);
  float cx = cameraMatrix.at<double>(0, 2);
  float cy = cameraMatrix.at<double>(1, 2);

  //cv::Point2d principalPoint = _int.getPrincipalPoint();
  //float cx = principalPoint.x;
  //float cy = principalPoint.y;

  p2.x = (p3.x * fx / p3.z) + cx;
  p2.y = (p3.y * fy / p3.z) + cy;
};

void ofApp::project_on_ps3( const ofVec3f& p3, ofVec2f& p2 )
{
  const ofxCv::Intrinsics& _int = calib_ps3.getDistortedIntrinsics();
  //const ofxCv::Intrinsics& _int = calib_ps3.getUndistortedIntrinsics();

  cv::Mat cameraMatrix = _int.getCameraMatrix();

  float fx = cameraMatrix.at<double>(0, 0);
  float fy = cameraMatrix.at<double>(1, 1);
  float cx = cameraMatrix.at<double>(0, 2);
  float cy = cameraMatrix.at<double>(1, 2);

  //cv::Point2d principalPoint = _int.getPrincipalPoint();
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

void ofApp::load_aruco_kinect_calib(string filename) 
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

  int_k.setup( cameraMatrix, imageSize, sensorSize ); 
  calib_kinect.setIntrinsics( int_k, distCoeffs ); //fires calib_xxx.updateUndistortion();
}

void ofApp::load_aruco_ps3_calib(string filename) 
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

  int_ps3.setup( cameraMatrix, imageSize, sensorSize );
  calib_ps3.setIntrinsics( int_ps3, distCoeffs ); //fires calib_xxx.updateUndistortion();
}

void ofApp::load_stereo_calib(string filename)
{
  bool absolute = false;

  cv::FileStorage fs( ofToDataPath(filename, absolute), cv::FileStorage::READ );

  fs["R"] >> calib_stereo.R;
  fs["T"] >> calib_stereo.T;

  if ( calib_stereo.R.type() != CV_32FC1 ) calib_stereo.R.convertTo( calib_stereo.R, CV_32FC1 );

  if ( calib_stereo.T.type() != CV_32FC1 ) calib_stereo.T.convertTo( calib_stereo.T, CV_32FC1 );


  //@#$%ˆ& scale.....
  float scale = 0.01;

  calib_stereo.T *= scale;


  //ofQuaternion q;
  ////m.Rvec[a,b,c]
  //float theta = sqrt(a^2 + b^2 + c^2);
  //ofVec3f axis = [a/theta, b/theta, c/theta]; 
  //q.makeRotate( theta, axis );
  //calib_stereo.MV.setRotate(q);


  //from Aruco CameraParameters::getCameraLocation
  calib_stereo.MV = cv::Mat::eye(4,4,CV_32FC1);

  //rotation
  for (int i=0;i<3;i++)
    for (int j=0;j<3;j++)
      calib_stereo.MV.at<float>(i,j) = calib_stereo.R.at<float>(i,j);

  //now, add the translation
  for (int i=0;i<3;i++)
    calib_stereo.MV.at<float>(i,3) = calib_stereo.T.at<float>(i,0);

  //calib_stereo.MV.inv(); 


  //from Aruco Marker::glGetModelViewMatrix

  //rotation
  float para[3][4];
  for (int i=0;i<3;i++)
    for (int j=0;j<3;j++) 
      para[i][j] = calib_stereo.R.at<float>(i,j);

  //now, add the translation
  para[0][3] = calib_stereo.T.at<float>(0,0);
  para[1][3] = calib_stereo.T.at<float>(1,0);
  para[2][3] = calib_stereo.T.at<float>(2,0);

  // R1
  calib_stereo.glMV[0 + 0*4] = para[0][0];
  calib_stereo.glMV[0 + 1*4] = para[0][1];
  calib_stereo.glMV[0 + 2*4] = para[0][2];
  calib_stereo.glMV[0 + 3*4] = para[0][3];
  // R2
  calib_stereo.glMV[1 + 0*4] = para[1][0];
  calib_stereo.glMV[1 + 1*4] = para[1][1];
  calib_stereo.glMV[1 + 2*4] = para[1][2];
  calib_stereo.glMV[1 + 3*4] = para[1][3];
  // R3
  calib_stereo.glMV[2 + 0*4] = -para[2][0];
  calib_stereo.glMV[2 + 1*4] = -para[2][1];
  calib_stereo.glMV[2 + 2*4] = -para[2][2];
  calib_stereo.glMV[2 + 3*4] = -para[2][3];
  // R4
  calib_stereo.glMV[3 + 0*4] = 0.0;
  calib_stereo.glMV[3 + 1*4] = 0.0;
  calib_stereo.glMV[3 + 2*4] = 0.0;
  calib_stereo.glMV[3 + 3*4] = 1.0;

  if (scale != 0.0)
  {
    calib_stereo.glMV[12] *= scale;
    calib_stereo.glMV[13] *= scale;
    calib_stereo.glMV[14] *= scale;
  }


  //copy to of mv
  //for (int i=0;i<16;i++)
    //calib_stereo.ofMV.getptr()[i] = calib_stereo.glMV[i];

  //copy to of MV
  for (int i=0;i<4;i++)
  for (int j=0;j<4;j++)
    calib_stereo.ofMV(i,j) = calib_stereo.MV.at<float>(i,j);


  calib_stereo.ofMV = ofMatrix4x4::getTransposedOf( calib_stereo.ofMV );
  //calib_stereo.ofMV.scale( 1, 1, -1 );

  ofLog()
      << "\n calib_stereo.T = \n" << calib_stereo.T
      << "\n calib_stereo.R = \n" << calib_stereo.R
      << "\n calib_stereo.MV = \n" << calib_stereo.MV
      << "\n calib_stereo.ofMV = \n" << calib_stereo.ofMV
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
    draw_ps3eye = !draw_ps3eye;
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
