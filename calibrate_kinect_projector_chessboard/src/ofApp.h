#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxCamaraLucidaCalibration.h"
#include "ofxOsc.h"
#include "ofxJSON.h"

#include "data_path.h"

class ofApp : public ofBaseApp
{

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

    cml::ProjectorCameraCalibration calibration;
    ofxJSONElement settings;
    ofxOscReceiver receiver;

    void update_osc();
    void log_osc_msg( ofxOscMessage& m );

    void update_win();

    float chessboard_brightness;
    bool chessboard_projected;

};

