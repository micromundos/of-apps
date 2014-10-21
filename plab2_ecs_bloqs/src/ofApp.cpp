#include "ofApp.h"

void ofApp::setup()
{
  ofSetLogLevel(OF_LOG_NOTICE);
  //ofSetLogLevel("BloqsManager", OF_LOG_NOTICE);

  ECS::FPS = 60.;
  ofSetFrameRate( ECS::FPS ); 
  ofSetVerticalSync(true);

  if ( ! config.open("config.json") )
  {
    ofLogError() << "error opening config.json";
    return;
  }


  kinect.setRegistration(false);
  // ir, rgb, texture
  kinect.init(false, true, true);
  kinect.open();


  bloqs_input.init( kinect.width,kinect.height );

  ecs.init();
  add_systems();
  ecs.init_systems();

  sim_man.init( &ecs, &config );
  sim_man.make_entity(); //LA simulazione

  bloqs_man.init( &ecs, &config );

  ofAddListener( bloqs_input.added, this, &ofApp::bloq_added );
  ofAddListener( bloqs_input.updated, this, &ofApp::bloq_updated );
  ofAddListener( bloqs_input.removed, this, &ofApp::bloq_removed );

}

void ofApp::add_systems()
{
  ecs.add_system( new ParticleSystem() );
  ecs.add_system( new ParticleEmitterSystem() );
  //ecs.add_system( new RenderSystem() );
};

void ofApp::update()
{
  ofSetWindowTitle(ofToString(ofGetFrameRate(),2));

  kinect.update();
  if ( ! kinect.isFrameNew() )
    return;

  bloqs_input.update( kinect.getPixelsRef() );
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
  bloqs_input.render();
}



void ofApp::bloq_added( Bloq& bloq )
{
  artemis::Entity* e = bloqs_man.make_entity( bloq );
}

void ofApp::bloq_updated( Bloq& bloq )
{
  bloqs_man.update_entity( bloq );
}

void ofApp::bloq_removed( string& bloq_id )
{
  bloqs_man.remove_entity( bloq_id );
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

