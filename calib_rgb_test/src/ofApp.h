#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxAruco.h"
#include "ofxPS3EyeGrabber.h"
#include "ofxCv.h"

struct Extrinsics
{
  cv::Mat R, T;
  cv::Mat MV;
  float glMV[16];
  ofMatrix4x4 ofMV;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);


    int w, h;
    ofxKinect kinect;
		ofxAruco aruco;
    ofxPS3EyeGrabber ps3;
    ofTexture ps3_tex;

    //TODO use cml::OpticalDevice::project
    void project_on_kinect( const ofVec3f& p3, ofVec2f& p2 );
    void project_on_ps3( const ofVec3f& p3, ofVec2f& p2 );

    ofMatrix4x4 get_marker_mv(aruco::Marker& m);
    void draw_kinect_undistorted( int x, int y );

    //from ofxCv::Calibration::load
    ofxCv::Intrinsics int_k;
    ofxCv::Calibration calib_kinect;
    void load_aruco_kinect_calib();

    ofxCv::Intrinsics int_ps3;
    ofxCv::Calibration calib_ps3;
    void load_aruco_ps3_calib();

    Extrinsics calib_stereo;
    void load_stereo_calib();

    void drawMarker( float size, const ofColor& color );

    bool draw_ps3eye;

};
