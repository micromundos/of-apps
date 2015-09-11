#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxGui.h"
#include "oscParamSender.h"
#include "params/ECSparamsSender.h"
#include "params/ParamID.h"

#include "data_path.h"

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

    bool send_params;

    //ofParameterGroup params;
    ofParameterGroup params_motor, params_game;

    //ofxPanel gui;
    ofxPanel gui_motor, gui_game;
    //string gui_motor_settings, gui_game_settings;

    //string gui_settings;
    ofxOscSender sender;

    ofxJSONElement motor, game, settings;
    string settings_path, game_path, motor_path;

    ECSparamsSender params_sender;

    float padding;
    float panel_width;
    void scroll_gui( ofxPanel& gui );

    void load_all_config();
    void parse_all_config();
    void save_all_config();
    void load_and_parse_all_config();

    void parse_config( ofxJSONElement& config, ofParameterGroup& params );
    void update_config( ofxJSONElement& config, ofParameterGroup& params );
    void save_config( ofxJSONElement& config, string filename, ofParameterGroup& params );

    Json::Value* get_entity( ofxJSONElement& config, string _entity_id );
    Json::Value* get_component( Json::Value& entity, string _component_id );
    void set_data( ofAbstractParameter& param, Json::Value& data );

};

