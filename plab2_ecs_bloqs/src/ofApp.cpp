#include "ofApp.h"
#include "add_systems.h"

void ofApp::setup()
{
  ofSetLogLevel(OF_LOG_NOTICE);
  //ofSetLogLevel("BloqsManager", OF_LOG_NOTICE);

  ofSetFrameRate( ECS::FPS ); 
  ofSetVerticalSync(true);

  if ( ! config.open("config.json") )
  {
    ofLogError() << "error opening config.json";
    return;
  }

  ecs.init();
  add_systems();
  ecs.init_systems();

  escena.init( &ecs, &config, "escena" );
  bloqs.init( &ecs, &config );

  BloqEventsComponent* bloq_events = ecs.component<BloqEventsComponent>("escena");

  ofAddListener( bloq_events->added, this, &ofApp::bloq_added );
  ofAddListener( bloq_events->updated, this, &ofApp::bloq_updated );
  ofAddListener( bloq_events->removed, this, &ofApp::bloq_removed );

}

void ofApp::update()
{
  ofSetWindowTitle(ofToString(ofGetFrameRate(),2));
}

void ofApp::draw()
{
  ofBackground(100,100,100); 
  ecs.update();
}


void ofApp::bloq_added( Bloq& bloq )
{
  bloqs.make_entity( bloq );
}

void ofApp::bloq_updated( Bloq& bloq )
{
  bloqs.update_entity( bloq );
}

void ofApp::bloq_removed( string& bloq_id )
{
  bloqs.remove_entity( bloq_id );
}


void ofApp::keyPressed(int key)
{

}


void ofApp::keyReleased(int key)
{

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

