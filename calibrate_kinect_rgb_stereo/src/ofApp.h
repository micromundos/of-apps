#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxCamaraLucidaCalibration.h"
#include "ofxJSON.h"

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
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    cml::StereoCalibration calibration;
    ofPixels pix_kinect_rgb, pix_rgb;

    ofxKinect kinect;
    ofVideoGrabber rgb_cam;

    string rgb_cam_name;
    int rgb_device_id;
    int rgb_width, rgb_height;
    int rgb_width_draw, rgb_height_draw;

    ofxJSONElement settings;
};
