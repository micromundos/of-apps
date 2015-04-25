#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxCamaraLucidaCalibration.h"

#include "data_path.h"

class ofApp : public ofBaseApp 
{

  public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
		void keyReleased(int key);

    ofxKinect kinect;
    ofPixels pix_kinect_rgb;

    ofImage undistorted;
    ofPixels previous;
    ofPixels diff;
    float diffMean;

    float lastTime;
    bool active;

    ofxCv::Calibration calibration;
    cml::Calibration calibration_cml;

    int w, h;

    void save_all();
    bool load_pattern_settings( string pattern_settings_file );
};

