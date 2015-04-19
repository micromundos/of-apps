#include "ofApp.h"

void ofApp::setup()
{
  ofSetDataPathRoot( __data_path__ );
  ofSetFrameRate(30);
  ofSetVerticalSync(true);
  ofSetLogLevel(OF_LOG_NOTICE);

  if ( !settings.open( "config/settings.json" ) ) 
  {
    ofLogError() << "error opening settings.json";
    return;
  }

  calibration.init(
      settings["params"]["calib_kinect_projector"]["calib_cam_path"].asString(), 
      settings["params"]["calib_kinect_projector"]["pattern_settings_path"].asString(), 
      settings["params"]["calib_kinect_projector"]["cam_name"].asString(), 
      settings["params"]["calib_kinect_projector"]["proj_name"].asString() );

  chessboard_brightness = 127;
  chessboard_projected = true;

  ofLog() << "chessboard window init";
  ofLog() 
    << "calib config" 
    << "\n" 
    << calibration.log_config();

  update_win(); 

  receiver.setup( settings["params"]["calib_kinect_projector"]["chessboard_port"].asInt() ); 
}

void ofApp::update()
{
  update_osc();
}

void ofApp::draw()
{
  calibration.render_chessboard( 0, 0, chessboard_brightness );
}

void ofApp::update_win()
{
  ofSetWindowShape( 
      calibration.proj_size().width,
      calibration.proj_size().height );

  ofSetWindowPosition( chessboard_projected ? ofGetScreenWidth() : ofGetScreenWidth() - calibration.proj_size().width, 0 );
}

void ofApp::update_osc()
{
  while( receiver.hasWaitingMessages() )
  {
    ofxOscMessage m;
    receiver.getNextMessage(&m);

    //log_osc_msg( m );

    if ( m.getAddress() == "/chessboard_brightness" )
    {
      chessboard_brightness = m.getArgAsFloat(0);
    } 

    else if ( m.getAddress() == "/chessboard_projected" )
    {
      chessboard_projected = m.getArgAsInt32(0);
      update_win();
    }
  }
};


void ofApp::log_osc_msg( ofxOscMessage& m )
{
  ofLog() << "osc msg";
  ofLog() << "addr: " << m.getAddress();
  for ( int i = 0; i < m.getNumArgs(); i++)
  {
    ofLog() << "\t arg type: " << m.getArgTypeName(i);
    if ( m.getArgType(i) == OFXOSC_TYPE_INT32 )
      ofLog() << "\t\t" << ofToString( m.getArgAsInt32(i) );
    else if ( m.getArgType(i) == OFXOSC_TYPE_FLOAT )
      ofLog() << "\t\t" << ofToString( m.getArgAsFloat(i) );
    else if ( m.getArgType(i) == OFXOSC_TYPE_STRING )
      ofLog() << "\t\t" << m.getArgAsString(i);
    else
      ofLog() << "\t\t unknown";
  }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
