#include "ofApp.h"


void ofApp::setup(){

  ofSetDataPathRoot( __data_path__ );

  ofSetFrameRate( 30 ); 
	ofSetVerticalSync(true); 

  load_jsons();

  string host = "localhost";
  int app_port = settings["params"]["app_port"].asInt();
  //int ctrl_port = settings["params"]["ctrl_port"].asInt();

  sender.setup( host, app_port );

  params_sender.init( &sender );
	params.setName("params");

  parse_config( motor );
  parse_config( game );

  ofxGuiSetDefaultWidth(ofGetWidth()-20);
  gui.setup( params );
}


void ofApp::parse_config( ofxJSONElement& config )
{
  ofLog() << "*** parse config ***";

  Json::Value::Members members( config.getMemberNames() );
  for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it) 
  {

    const std::string &key = *it;
    ofLog() << "\t" << "config key: " << key;
    Json::Value& entities = config[key];

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
        if ( !has_data ) continue;

        Json::Value& component_data = component["data"];

        Json::Value::Members data_members( component_data.getMemberNames() );
        for ( Json::Value::Members::iterator it = data_members.begin(); it != data_members.end(); ++it ) 
        {
          const std::string &key = *it;

          const Json::Value& value = component_data[key];

          //see ECScomponent::param
          string param_id = ParamID::make( entity_id, component_id, key );

          ofLog() 
            << "\t\t\t\t" 
            << "component data" 
            << "\n" << "\t\t\t\t\t\t" 
            << "key: " << key 
            << "\n" << "\t\t\t\t\t\t" 
            << "value as str: " << ( !value.isArray() ? value.asString() : "[array]" ) 
            << "\n" << "\t\t\t\t\t\t" 
            << "param_id " << param_id;

          params_sender.add( param_id, value, params ); 

        }
      }
    }
  }
};


void ofApp::load_jsons()
{
  if ( !settings.open( "config/settings.json") )
    ofLogError() << "error opening settings.json";

  if ( !game.open( "config/game.json") )
    ofLogError() << "error opening game.json";

  if ( !motor.open( "config/motor.json") )
    ofLogError() << "error opening motor.json";
};


void ofApp::update(){
}


void ofApp::draw(){
  gui.draw();
}


void ofApp::exit(){
  params.clear();
  gui.clear();
}


void ofApp::keyPressed(int key){

}


void ofApp::keyReleased(int key){

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

