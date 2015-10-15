#include "ofApp.h"

void ofApp::setup()
{
  ofSetDataPathRoot( __data_path__ );
  ofSetWindowPosition(0,0);

  ofSetLogLevel(OF_LOG_NOTICE);

  //ofSetFrameRate(60.0f); 
  ofSetVerticalSync(false);

  keys::init();
  //gpgpu::Process::watch("all");
  init_time_measurements();

  config.init();

  ofSetWindowShape(
    config.settings()["params"]["app"]["width"].asInt(),
    config.settings()["params"]["app"]["height"].asInt()
  );

  ecs.init();
  ecs.fps(30.0);
  system_factory.add_systems( ecs );
  ecs.init_systems();

  component_factory = new PlabComponentFactory();

  int app_port = config.settings()["params"]["app"]["port"].asInt();

  motor.init( ecs.get_world(), ((ComponentFactory*)component_factory), config.motor()["motor"], app_port );
  motor.make_all();

  game.init( ecs.get_world(), ((ComponentFactory*)component_factory), config.game()["game"], app_port );
  game.make_all();

  cml_data = ecs.require_component<CamaraLucidaComponent>("output");
  ofAddListener( cml_data->cml->render_texture, this, &ofApp::render_texture );
}

void ofApp::update()
{
  ofSetWindowTitle( "of " + ofToString(ofGetFrameRate(),2) + " / ecs " + ofToString(ecs.framerate(),2) );
  ecs.update();
}

void ofApp::draw()
{
  ofBackground(100,100,100); 

  //pipe render to camara lucida
  if ( cml_data->enabled )
    cml_data->cml->render();
  else
    ecs.render();

  if ( render_ecs_fps )
    ofDrawBitmapString( "ecs fps " + ofToString(ecs.framerate(),2), ofGetWidth()-140, ofGetHeight()-10);
}

void ofApp::render_texture(ofEventArgs &args)
{
  ecs.render();
}

void ofApp::exit()
{
}

void ofApp::keyPressed(int key)
{
}


void ofApp::keyReleased(int key)
{
  switch (key)
  { 

    case keys::cml_key_debug:
      render_ecs_fps = !render_ecs_fps;
      break;

    case keys::projector:
      ofSetWindowPosition( ofGetWindowPositionX() == 0 ? ofGetScreenWidth() : 0, 0 );
      break;

    //case keys::fullscreen:
      //ofToggleFullscreen();
      //break;

    case keys::tm_toggle:
      TIME_SAMPLE_SET_ENABLED( !TIME_SAMPLE_GET_ENABLED() );
      break;
 
  };
}


void ofApp::init_time_measurements()
{
  TIME_SAMPLE_SET_FRAMERATE( 60.0f );

  TIME_SAMPLE_SET_DRAW_LOCATION( TIME_MEASUREMENTS_TOP_RIGHT );

  //averaging samples, (0..1]
  //1.0 gets you no averaging at all
  //use lower values to get steadier readings
  TIME_SAMPLE_SET_AVERAGE_RATE(0.1);
  //TIME_SAMPLE_DISABLE_AVERAGE();

  //inactive threads will be dropped from the table
  TIME_SAMPLE_SET_REMOVE_EXPIRED_THREADS(true);

  //customize color
  //TIME_SAMPLE_GET_INSTANCE()->setHighlightColor(ofColor::yellow);

  TIME_SAMPLE_SET_ENABLED( false );
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

