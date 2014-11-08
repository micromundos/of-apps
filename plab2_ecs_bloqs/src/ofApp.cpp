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

  entities.init( ecs.world(), &config, "entities" );
  entities.make_all();

  ofAddListener( ecs.component<CamaraLucidaComponent>("output")->cml->render_texture, this, &ofApp::render_texture );
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
  if (cml_render)
    ecs.component<CamaraLucidaComponent>("output")->cml->render();
  else
    ecs.render();
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
  cml::CamaraLucida* cml = ecs.component<CamaraLucidaComponent>("output")->cml;
  RenderComponent* render_data = ecs.component<RenderComponent>("output");

  switch (key)
  { 

    case keys::cml_render:

      cml_render = !cml_render;

      float w, h;
      if ( cml_render )
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

    case keys::projector:
      ofSetWindowPosition( ofGetWindowPositionX() == 0 ? ofGetScreenWidth()+1 : 0, 0 );
      break;

    case keys::fullscreen:
      ofToggleFullscreen();
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

