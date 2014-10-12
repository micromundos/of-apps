#include "ofApp.h"


void ofApp::setup()
{
  ofSetLogLevel(OF_LOG_NOTICE);
  //ofSetLogLevel("ECSbloqs", OF_LOG_NOTICE);

  ofSetFrameRate(60); 
  ofSetVerticalSync(true);

  kinect.setRegistration(false);
  // ir, rgb, texture
  kinect.init(false, true, true);
  kinect.open();

  bloqs.init(kinect.width,kinect.height);
  ecs.init();
  ecs.add_system( new ParticleSystem() );
  ecs.add_system( new RenderSystem() );
  ecs.init_systems();
  ecsbloqs.init( &ecs );

  ofAddListener( bloqs.added, this, &ofApp::bloq_added );
  ofAddListener( bloqs.removed, this, &ofApp::bloq_removed );
  ofAddListener( bloqs.updated, this, &ofApp::bloq_updated );

}

void ofApp::update()
{
  ofSetWindowTitle(ofToString(ofGetFrameRate(),2));

  kinect.update();
  if ( ! kinect.isFrameNew() )
    return;

  bloqs.update( kinect.getPixelsRef() );
}

void ofApp::draw()
{
	ofBackground(100,100,100);

  ofSetColor(255);
  int w = kinect.width;
  int h = kinect.height;
  kinect.draw( 0, 0, w, h );
  kinect.drawDepth( 0, 480, w, h );

  ecs.update();

  //debug
  bloqs.render();
}



void ofApp::bloq_added( Bloq& bloq )
{
  artemis::Entity* e = ecsbloqs.make_entity( bloq );
}

void ofApp::bloq_removed( Bloq& bloq )
{
  ecsbloqs.remove_entity( bloq );
}

void ofApp::bloq_updated( Bloq& bloq )
{
  ecsbloqs.update_entity( bloq );
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

