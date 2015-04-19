#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxAruco.h"
#include "ofxPS3EyeGrabber.h"
#include "ofxCv.h"

#include "data_path.h"

struct Extrinsics
{
  cv::Mat R, T;
  cv::Mat E, F;
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

    void project(const ofxCv::Intrinsics& intrinsics, const ofVec3f& p3, ofVec2f& p2);

    ofMatrix4x4 get_marker_mv(aruco::Marker& m);
    void draw_kinect_undistorted( int x, int y );

    //from ofxCv::Calibration::load
    ofxCv::Intrinsics int_k;
    ofxCv::Calibration calib_kinect;
    void load_aruco_kinect_intrinsics(string filename);

    ofxCv::Intrinsics int_ps3;
    ofxCv::Calibration calib_ps3;
    void load_aruco_ps3_intrinsics(string filename);

    Extrinsics extrinsics;
    vector<cv::Vec3f> epilines;
    void load_extrinsics(string filename);

    void drawMarker( float size, const ofColor& color );

    void draw_epilines();

    bool draw_ps3eye;

};
