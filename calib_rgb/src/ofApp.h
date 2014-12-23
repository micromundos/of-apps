#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxPS3EyeGrabber.h"

class ofApp : public ofBaseApp 
{
  public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    ofxCv::Calibration calib_kinect;
    ofxCv::Calibration calib_ps3eye;

    ofImage undistorted_kinect;
    ofImage undistorted_ps3eye;

    ofPixels previous_kinect;
    ofPixels previous_ps3eye;

    ofPixels diff_kinect;
    ofPixels diff_ps3eye; 

    ofPixels pix_kinect;
    ofPixels pix_ps3eye;

    cv::Mat camMat_kinect;
    cv::Mat camMat_ps3eye;

    bool update_kinect();
    bool update_ps3eye();

    bool update_camMat( cv::Mat& camMat, ofPixels& pix, ofPixels& previous, ofPixels& diff, float* diffMean );

    bool update_calib( string name, ofxCv::Calibration& calibration, cv::Mat& camMat, ofPixels& pix, ofImage& undistorted );

    bool find_board( ofxCv::Calibration& calibration, cv::Mat& camMat );

    float diffMean_kinect;
    float diffMean_ps3eye;

    float curTime;
    float lastTime;

    bool capture;

    int w, h, chan;

    cv::FileStorage calib_settings;
    cv::FileStorage load_calib_settings();
    void init_calib();
    void allocate();

    ofxKinect kinect;
    ofxPS3EyeGrabber ps3;
    ofTexture ps3_tex; 

    void render_calib( ofxCv::Calibration& calibration, int x );
    void debug_calib( ofxCv::Calibration& calibration, string name, int x );

    //see ofxCalibration::save
    void calib_save_aruco_format( ofxCv::Calibration& calibration, string name );
    void calib_save_stereo_RT( string src_name, ofxCv::Calibration& src_calib, string dst_name, ofxCv::Calibration& dst_calib );

};
