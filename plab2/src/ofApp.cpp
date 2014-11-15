#include "ofApp.h"

void ofApp::setup()
{
  ofSetLogLevel(OF_LOG_NOTICE);

  //TODO set ECS::FPS
  ofSetFrameRate( ECS::FPS ); 
  ofSetVerticalSync(true);

  ecs.init();
  system_factory.add_systems( ecs );
  ecs.init_systems();

  component_factory = new PlabComponentFactory();
  motor.init( ecs.world(), ((ComponentFactory*)component_factory), config.json()["motor"] );
  motor.make_all();

  cml_data = ecs.component<CamaraLucidaComponent>("output");
  if (cml_data)
    ofAddListener( cml_data->cml->render_texture, this, &ofApp::render_texture );
  else
    ofLogError() << "CamaraLucidaComponent is NULL";
}

void ofApp::update()
{
  ofSetWindowTitle(ofToString(ofGetFrameRate(),2));
  ecs.update();
}

void ofApp::draw()
{
  ofBackground(255,255,255); 

  //pipe render to camara lucida
  if ( cml_data && cml_data->enabled )
    cml_data->cml->render();
  else
    ecs.render();
}

void ofApp::exit()
{
}

void ofApp::render_texture(ofEventArgs &args)
{
  ecs.render();
}

void ofApp::keyPressed(int key)
{
}


void ofApp::keyReleased(int key)
{

  cml::CamaraLucida* cml = cml_data->cml;

  RenderComponent* render_data = ecs.component<RenderComponent>("output");

  switch (key)
  { 

    case keys::cml_enabled:

      cml_data->toggle();

      float w, h;
      if ( cml_data->enabled )
      {
        w = cml->tex_width();
        h = cml->tex_height();
      }

      else 
      {
        w = ofGetWidth();
        h = ofGetHeight();
      }

      render_data->update( w, h );

      break;

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

