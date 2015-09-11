#include "ofApp.h"


int prev;
int send_interval;

void ofApp::setup()
{
  ofSetDataPathRoot( __data_path__ );
  ofSetWindowPosition( ofGetScreenWidth()-ofGetWidth()-20, 0 );

  ofSetFrameRate( 30 ); 
  ofSetVerticalSync(true); 

  send_params = true;

  load_all_config();

  int app_port = settings["params"]["app"]["port"].asInt();
  string host = settings["params"]["ctrl"]["host"].asString();
  //int ctrl_port = settings["params"]["ctrl"]["port"].asInt();

  send_interval = settings["params"]["ctrl"]["send_interval"].asInt();

  sender.setup( host, app_port );

  params_sender.init( &sender );

	//params.setName("params");
  params_motor.setName("motor");
  params_game.setName("game");

  parse_all_config();


  padding = 10;
  panel_width = ofGetWidth()/2 - padding*1.5;

  ofxGuiSetDefaultWidth( panel_width );

  //gui_motor_settings = "config/gui_motor.xml";
  //gui_game_settings = "config/gui_game.xml";

  gui_motor.setup( params_motor ); //, gui_motor_settings );
  //gui_motor.loadFromFile( gui_motor_settings );
  gui_motor.setPosition( panel_width + padding*2, padding );

  gui_game.setup( params_game ); //, gui_game_settings );
  //gui_game.loadFromFile( gui_game_settings );
  gui_game.setPosition( padding, padding );

  //gui.setup( params );
  //gui_settings = "ctrl/gui.xml";
  //gui.setup( params, gui_settings );
  //gui.loadFromFile( gui_settings );

  //ofAddListener( gui.savePressedE, this, &ofApp::save_all_config );
  //ofAddListener( gui.loadPressedE, this, &ofApp::load_and_parse_all_config );
  ofAddListener( gui_motor.savePressedE, this, &ofApp::save_all_config );
  ofAddListener( gui_motor.loadPressedE, this, &ofApp::load_and_parse_all_config );
  ofAddListener( gui_game.savePressedE, this, &ofApp::save_all_config );
  ofAddListener( gui_game.loadPressedE, this, &ofApp::load_and_parse_all_config );

  prev = ofGetSeconds();
}

void ofApp::update()
{
  int now = ofGetSeconds();
  if ( send_params && abs(now-prev) >= send_interval )
  {
    params_sender.send();
    prev = now;
  }

  //scroll_gui( gui, padding );
  scroll_gui( gui_motor );
  scroll_gui( gui_game );
}

void ofApp::scroll_gui( ofxPanel& gui )
{
  float yoff = padding * 2;
  if ( gui.getShape().height > ofGetHeight() - yoff )
  {
    float pct = ofMap( ofGetMouseY(), 0, ofGetHeight(), 0, 1, true );
    float diff = gui.getShape().height - ofGetHeight()+yoff*2;
    gui.setPosition( gui.getPosition().x, -diff * pct +yoff );
  }
}

void ofApp::parse_all_config()
{
  parse_config( motor, params_motor );
  parse_config( game, params_game );
};

void ofApp::load_all_config()
{
  settings_path = "config/settings.json"; 

  if ( !settings.open( settings_path ) )
    ofLogError() << "error opening settings.json";

  game_path = settings["params"]["app"]["game_path"].asString();
  motor_path = settings["params"]["app"]["motor_path"].asString();

  if ( !game.open( game_path ) )
    ofLogError() << "error opening game.json";

  if ( !motor.open( motor_path ) )
    ofLogError() << "error opening motor.json";
};

void ofApp::save_all_config()
{
  save_config( motor, motor_path, params_motor );
  save_config( game, game_path, params_game );

  //gui_motor.saveToFile( gui_motor_settings );
  //gui_game.saveToFile( gui_game_settings );
};

void ofApp::load_and_parse_all_config()
{
  load_all_config();
  parse_all_config();
};


void ofApp::parse_config( ofxJSONElement& config, ofParameterGroup& params )
{
  ofLog() << "*** parse config ***";

  Json::Value::Members members( config.getMemberNames() );
  for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it) 
  {

    const std::string &bundle_id = *it;
    ofLog() << "\t" << "config bundle_id: " << bundle_id;
    Json::Value& entities = config[bundle_id];

    for (int i = 0; i < entities.size(); i++)
    {
      Json::Value& entity = entities[i];
      string entity_id = entity["id"].asString();
      ofLog() << "\t\t" << "entity " << entity_id;

      Json::Value& components = entity["components"];

      for (int j = 0; j < components.size(); j++)
      {
        Json::Value& component = components[j];
        string component_id = component["id"].asString();
        bool has_data = component.isMember("data");
        ofLog() << "\t\t\t" << "component " << component_id << ", has data: " << has_data;

        if ( !has_data ) 
          continue;

        Json::Value& component_data = component["data"];

        Json::Value::Members data_members( component_data.getMemberNames() );
        for ( Json::Value::Members::iterator it = data_members.begin(); it != data_members.end(); ++it ) 
        {
          const std::string &data_id = *it;

          const Json::Value& value = component_data[data_id];

          //see ECScomponent::param
          string param_id = ParamID::make( entity_id, component_id, data_id );

          ofLog() 
            << "\t\t\t\t" 
            << "component data" 
            << "\n" << "\t\t\t\t\t\t" 
            << "data_id: " << data_id 
            << "\n" << "\t\t\t\t\t\t" 
            << "value as str: " << ( value.isArray() ? value[0] : value ) 
            << "\n" << "\t\t\t\t\t\t" 
            << "param_id " << param_id;

          params_sender.add( param_id, value, params );
        }
      }
    }
  }
};


void ofApp::update_config( ofxJSONElement& config, ofParameterGroup& params )
{
  ofLog() << "*** update config ***";

  for ( int i = 0; i < params.size(); i++ )
  {
    ofAbstractParameter& param = params[i];
    string _param_id = param.getName();
    string _param_type = param.type();
    //string _param_id = params_added[i]; 

    string _entity_id = ParamID::entity_id( _param_id );
    string _component_id = ParamID::component_id( _param_id );
    string _data_id = ParamID::data_id( _param_id );

    ofLog() 
      << "param id: " << _param_id 
      << ", type: " << _param_type
      << ", entity id: " << _entity_id
      << ", component id: " << _component_id
      << ", data id: " << _data_id;

    Json::Value* entity = get_entity( config, _entity_id );
    if ( entity == NULL ) continue; 
    Json::Value* component = get_component( *entity, _component_id );
    if ( component == NULL ) continue; 
    Json::Value* data = &(*component)["data"][_data_id];
    if ( data == NULL ) continue;

    ofLog() 
      << "\t ptrs->"
      << " entity: " << entity
      << ", component: " << component
      << ", data: " << data;

    set_data( param, *data );
    ofLog() << "\t\t set data: " << *data;

  }
};

void ofApp::save_config( ofxJSONElement& config, string filename, ofParameterGroup& params )
{
  ofLog() << "*** save config to " << filename << " ***";
  update_config( config, params );
  config.save( filename, true );
};

void ofApp::set_data( ofAbstractParameter& p, Json::Value& data )
{
  Json::Value& d = data.isArray() ? data[0] : data;

  if ( p.type() == typeid(ofParameter<int>).name() ) 
    d = p.cast<int>().get();

  else if ( p.type() == typeid(ofParameter<bool>).name() ) 
    d = p.cast<bool>().get();

  else if ( p.type() == typeid(ofParameter<float>).name() ) 
    d = p.cast<float>().get();

  else if ( p.type() == typeid(ofParameter<double>).name() ) 
    d = p.cast<double>().get();

  else if ( p.type() == typeid(ofParameter<string>).name() )
    d = p.cast<string>().get();
};

Json::Value* ofApp::get_entity( ofxJSONElement& config, string _entity_id )
{
  Json::Value::Members members( config.getMemberNames() );
  for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it) 
  {

    const std::string &bundle_id = *it;
    Json::Value& entities = config[bundle_id];

    for (int i = 0; i < entities.size(); i++)
    {
      Json::Value& entity = entities[i];
      string entity_id = entity["id"].asString();

      if ( entity_id == _entity_id )
        return &entity;
    }
  }

  return NULL;
};

Json::Value* ofApp::get_component( Json::Value& entity, string _component_id )
{
  Json::Value& components = entity["components"];

  for (int j = 0; j < components.size(); j++)
  {
    Json::Value& component = components[j];
    string component_id = component["id"].asString(); 

    bool has_data = component.isMember("data");

    if ( !has_data ) 
      continue;

    if ( component_id == _component_id )
      return &component;
  }

  return NULL;
};


void ofApp::draw()
{
  //gui.draw();
  gui_motor.draw();
  gui_game.draw();
  ofDrawBitmapStringHighlight( "send params " + ofToString(send_params) + " (press 'p')", ofGetWidth()-220, ofGetHeight()-14, ofColor::yellow, ofColor::black );
}


void ofApp::exit()
{
  //ofRemoveListener( gui.savePressedE, this, &ofApp::save_all_config );
  //ofRemoveListener( gui.loadPressedE, this, &ofApp::load_and_parse_all_config );
  ofRemoveListener( gui_motor.savePressedE, this, &ofApp::save_all_config );
  ofRemoveListener( gui_motor.loadPressedE, this, &ofApp::load_and_parse_all_config );
  ofRemoveListener( gui_game.savePressedE, this, &ofApp::save_all_config );
  ofRemoveListener( gui_game.loadPressedE, this, &ofApp::load_and_parse_all_config );

  //params.clear();
  params_motor.clear();
  params_game.clear();

  //gui.clear();
  gui_motor.clear();
  gui_game.clear();
}


void ofApp::keyPressed(int key)
{

}


void ofApp::keyReleased(int key)
{

  if ( key == 's' )
  {
    save_all_config();
  }

  else if ( key == 'p' )
  {
    send_params = !send_params;
  }

}


void ofApp::mouseMoved(int x, int y ){

}


void ofApp::mouseDragged(int x, int y, int button){

}


void ofApp::mousePressed(int x, int y, int button){

}


void ofApp::mouseReleased(int x, int y, int button){

}


void ofApp::windowResized(int w, int h){

}


void ofApp::gotMessage(ofMessage msg){

}


void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

