#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxAruco.h"
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

    ofVideoGrabber rgb_cam;

    void project(const ofxCv::Intrinsics& intrinsics, const ofVec3f& p3, ofVec2f& p2);

    ofMatrix4x4 get_marker_mv(aruco::Marker& m);
    void draw_kinect_undistorted( int x, int y );

    ofxCv::Intrinsics int_k;
    ofxCv::Calibration calib_kinect;

    ofxCv::Intrinsics int_rgb;
    ofxCv::Calibration calib_rgb;

    void load_intrinsics( ofxCv::Calibration& calib, ofxCv::Intrinsics& intrinsics, string filename );

    Extrinsics extrinsics;
    vector<cv::Vec3f> epilines;
    void load_extrinsics(string filename);

    void drawMarker( float size, const ofColor& color );

    void draw_epilines();

    bool draw_rgb;

};
