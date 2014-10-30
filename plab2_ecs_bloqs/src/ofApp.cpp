#include "ofApp.h"

void ofApp::setup()
{
  ofSetLogLevel(OF_LOG_NOTICE);

  ofSetFrameRate( ECS::FPS ); 
  ofSetVerticalSync(true);

  if ( ! config.open("config.json") )
  {
    ofLogError() << "error opening config.json";
    return;
  }

  ecs.init();
  ecs.add_systems();
  ecs.init_systems();
  entities.make( &ecs, &config );
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


void ofApp::keyPressed(int key)
{
  switch (key)
  { 
    case 'p':
      break;
  };
}


void ofApp::keyReleased(int key)
{
  switch (key)
  { 
    case 'p':
      break;

    case '+':
    case '=':
      break;

    case '-':
      break;

  };
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

