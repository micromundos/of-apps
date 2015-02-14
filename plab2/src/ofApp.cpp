#include "ofApp.h"

void ofApp::setup()
{
  ofSetLogLevel(OF_LOG_NOTICE);

  //ofSetFrameRate(60.0f); 
  ofSetVerticalSync(true);

  ecs.init();
  system_factory.add_systems( ecs );
  ecs.init_systems();

  component_factory = new PlabComponentFactory();
  motor.init( ecs.get_world(), ((ComponentFactory*)component_factory), config.json()["motor"] );
  motor.make_all();

  cml_data = ecs.require_component<CamaraLucidaComponent>("output");
  ofAddListener( cml_data->cml->render_texture, this, &ofApp::render_texture );
}

void ofApp::update()
{
  ofSetWindowTitle(ofToString(ofGetFrameRate(),2));
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

  cml::CamaraLucida* cml = cml_data->cml;

  RenderComponent* render_data = ecs.require_component<RenderComponent>("output");

  switch (key)
  { 

    case keys::cml_gpu:
      cml->gpu( !cml->gpu() );
      break;

    case keys::cml_wireframe:
      cml->wireframe( !cml->wireframe() );
      break;

    case keys::cml_depth_xoff_inc:
      cml->depth_camera()->xoff++;
      break;
    case keys::cml_depth_xoff_dec:
      cml->depth_camera()->xoff--;
      break;


    case keys::projector:
      ofSetWindowPosition( ofGetWindowPositionX() == 0 ? ofGetScreenWidth()+1 : 0, 0 );
      break;

    case keys::fullscreen:
      ofToggleFullscreen();
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

