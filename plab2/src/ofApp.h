#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxECS.h"
#include "components/PlabComponentFactory.h"
#include "utils/Config.h"
#include "keys.h"

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
    ECmaker motor;
    Config config;
    PlabComponentFactory* component_factory;

    //TODO systems factory
    //instanciar solo los que se usan
    //segun config.json
    void add_systems();

    CamaraLucidaComponent* cml_data;
    bool cml_render;
    void render_texture(ofEventArgs &args);

};

