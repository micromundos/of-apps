#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxKinect.h"

#include "ecs/ECS.h"
#include "managers/SimManager.h"
#include "managers/BloqsManager.h"
#include "bloqs/BloqsAruco.h"
#include "systems/RenderSystem.h"
#include "systems/ParticleSystem.h"
#include "systems/ParticleEmitterSystem.h"

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

    ofxJSONElement config;

    ECS ecs;
    SimManager sim_man;
    BloqsManager bloqs_man;
    BloqsAruco bloqs_input;

    ofxKinect kinect;

    void bloq_added( Bloq& bloq );
    void bloq_updated( Bloq& bloq );
    void bloq_removed( string& bloq_id );

    void add_systems();

};

