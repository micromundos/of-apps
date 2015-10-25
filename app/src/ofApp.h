#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxECS.h"
#include "ecs/PlabComponentFactory.h"
#include "ecs/PlabSystemFactory.h"
#include "utils/Config.h"

#include "keys.h"
#include "data_path.h"

#include "ofxTimeMeasurements.h"
#include "ofxGPGPU.h"

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

    ECS ecs;
    ECmaker motor, game;
    Config config;
    PlabComponentFactory component_factory;
    PlabSystemFactory system_factory;

    CamaraLucidaComponent* cml_data;
    void render_texture(ofEventArgs &args);

    void init_time_measurements();
    bool render_ecs_fps;

};

