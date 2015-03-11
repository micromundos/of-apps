#include "ofApp.h"


void ofApp::setup(){

  ofSetFrameRate( 30 ); 
	ofSetVerticalSync(true);

  if ( !settings.open( "config/settings.json") )
    ofLogError() << "error opening settings.json";

  if ( !config.open( "config/config.json") )
    ofLogError() << "error opening config.json";


  string host = "localhost";
  int app_port = settings["params"]["app_port"].asInt();
  //int ctrl_port = settings["params"]["ctrl_port"].asInt();

  sender.setup( host, app_port );

  params_sender.init( &sender );

	params.setName("params");

  Json::Value::Members members( config.getMemberNames() );
  for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it) 
  {

    const std::string &key = *it;
    ofLog() << "config key: " << key;
    Json::Value& entities = config[key];

    for (int i = 0; i < entities.size(); i++)
    {
      Json::Value& entity = entities[i];
      string entity_id = entity["id"].asString();
      ofLog() << "-- entity " << entity_id;

      Json::Value& components = entity["components"];

      for (int j = 0; j < components.size(); j++)
      {
        Json::Value& component = components[j];
        string component_id = component["id"].asString();
        bool has_data = component.isMember("data");
        ofLog() << "---- component " << component_id << ", has data: " << has_data;
        if ( !has_data ) continue;

        Json::Value& component_data = component["data"];

        Json::Value::Members data_members( component_data.getMemberNames() );
        for ( Json::Value::Members::iterator it = data_members.begin(); it != data_members.end(); ++it ) 
        {
          const std::string &key = *it;

          const Json::Value& value = component_data[key];

          //see ECScomponent::param
          string param_id = ParamID::make( entity_id, component_id, key );

          ofLog() << "------ component data key: " << key << ", value as str: " << ( !value.isArray() ? value.asString() : "[array]" ) << " / param_id " << param_id;

          params_sender.add( param_id, value, params ); 

        }
      }
    }
  }

  ofxGuiSetDefaultWidth(ofGetWidth()-20);
  gui.setup( params );

}


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

