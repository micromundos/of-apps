#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ecs/ECS.h"
#include "ecs/ECSconfig.h"
#include "keys.h"

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

    ECS ecs;
    ECSconfig entities;
    ofxJSONElement config;

    bool cml_render;
    void render_texture(ofEventArgs &args);

};

