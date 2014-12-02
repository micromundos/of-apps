#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxGui.h"
#include "oscParamSender.h"
#include "params/ECSparamsSender.h"
#include "params/ParamID.h"

class ofApp : public ofBaseApp{

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

    ofParameterGroup params;
    ofxOscSender sender;
    ofxPanel gui;

    ofxJSONElement config;
    ofxJSONElement settings;

    ECSparamsSender params_sender;

};

