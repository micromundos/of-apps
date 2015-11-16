#include "ofApp.h"

int yoff = 0; //window undecorated yoffset

void ofApp::setup()
{
  ofSetDataPathRoot( __data_path__ );
  ofSetFrameRate(30);
  ofSetVerticalSync(true);
  ofSetLogLevel(OF_LOG_NOTICE); 

  if ( !settings.open( "config/settings.json" ) ) 
  {
    ofLogFatalError() << "error opening settings.json";
    ofExit();
    return;
  } 

  kinect.setRegistration(true);
  // ir, rgb, texture
  kinect.init(false, true, true);
  kinect.open();
  kinect.update(); 

  w = kinect.getWidth();
  h = kinect.getHeight();


  calibration.init( 
      settings["params"]["calib_kinect_projector"]["calib_cam_path"].asString(), 
      settings["params"]["calib_kinect_projector"]["pattern_settings_path"].asString(), 
      settings["params"]["calib_kinect_projector"]["cam_name"].asString(), 
      settings["params"]["calib_kinect_projector"]["proj_name"].asString() );

  ofLog() 
    << "calib config" 
    << "\n" 
    << calibration.log_config();

  //window setup
  ofSetWindowShape( 
      calibration.cam_size().width * 2,
      //+ calibration.proj_size().width, 
      ofGetScreenHeight() );
  //ofSetWindowPosition( ofGetScreenWidth() - calibration.cam_size().width, 0 );
  ofSetWindowPosition( 0, 0 );


  receiver.setup( 
      settings["params"]["calib_kinect_projector"]["port"].asInt() );

  sender.setup( 
      settings["params"]["calib_kinect_projector"]["chessboard_host"].asString(), 
      settings["params"]["calib_kinect_projector"]["chessboard_port"].asInt() );


  capture_osc_addr = "/capture";
  calibrate_osc_addr = "/calibrate"; 
  save_calib_osc_addr = "/save_calib";
  reset_calib_osc_addr = "/reset_calib";
  save_images_osc_addr = "/save_images";
  load_images_osc_addr = "/load_images";
  chessboard_projected_osc_addr = "/chess_proj";
  chessboard_brightness_osc_addr = "/chess_brightness";

  capture_btn.addListener(this,&ofApp::capture);
  calibrate_btn.addListener(this,&ofApp::calibrate);
  save_calib_btn.addListener(this,&ofApp::save_calib);
  save_images_btn.addListener(this,&ofApp::save_images);
  load_images_btn.addListener(this,&ofApp::load_images);
  reset_calib_btn.addListener(this,&ofApp::reset_calib);
  chessboard_brightness.addListener(this,&ofApp::chessboard_brightness_changed);
  chessboard_projected.addListener(this,&ofApp::chessboard_projected_changed);

  ofxBaseGui::setDefaultWidth( 400 );
  string gui_settings_file = settings["params"]["calib_kinect_projector"]["gui_settings_path"].asString();
;
	gui.setup( "", gui_settings_file );
  gui.add( chessboard_brightness.setup( "chessboard_brightness osc: "+chessboard_brightness_osc_addr, 127, 0, 255 ) );
  gui.add( chessboard_projected.setup( "chessboard_projected osc: "+chessboard_projected_osc_addr, true ) );
  gui.add( capture_btn.setup("capture (spacebar) osc: "+capture_osc_addr) );
  gui.add( calibrate_btn.setup("calibrate (c) osc: "+calibrate_osc_addr) );
  gui.add( save_calib_btn.setup("save calibration (s) osc: "+save_calib_osc_addr) );
  gui.add( save_images_btn.setup("save images (i) osc: "+save_images_osc_addr) );
  gui.add( load_images_btn.setup("load images (l) osc: "+load_images_osc_addr) );
  gui.add( reset_calib_btn.setup("reset calibration (r) osc: "+reset_calib_osc_addr) );

  gui.setPosition( 
    10, calibration.cam_size().height + 90
    //calibration.cam_size().width - gui.getWidth() - 10, 
    //calibration.cam_size().height + yoff + 10
  );
  gui.loadFromFile( gui_settings_file );
 
}


void ofApp::exit() 
{
  capture_btn.removeListener(this,&ofApp::capture);
  calibrate_btn.removeListener(this,&ofApp::calibrate);
  save_calib_btn.removeListener(this,&ofApp::save_calib);
  save_images_btn.removeListener(this,&ofApp::save_images);
  load_images_btn.removeListener(this,&ofApp::load_images);
  reset_calib_btn.removeListener(this,&ofApp::reset_calib);

  chessboard_brightness.removeListener(this,&ofApp::chessboard_brightness_changed);
  chessboard_projected.removeListener(this,&ofApp::chessboard_projected_changed);

  kinect.close();
}

void ofApp::update()
{
  ofSetWindowTitle(ofToString(ofGetFrameRate(),2));

  update_osc();

  kinect.update();

  if ( !kinect.isFrameNew() ) return;

  pix_kinect_rgb = kinect.getPixelsRef(); //copy
  calibration.update( pix_kinect_rgb );
}

void ofApp::draw()
{
  ofBackground(100);
  ofSetColor(255); 

  kinect.draw( 0, yoff, w, h );
  //kinect.drawDepth( 0, yoff+h, w, h );

  calibration.render( 0,h ); 
  calibration.render_capture( w,yoff, w,h );
  //calibration.render_chessboard( chessboard_projected ? calibration.cam_size().width : 0, 0, chessboard_brightness );

  gui.draw();
}


void ofApp::chessboard_brightness_changed(float &_chessboard_brightness)
{
  send_chessboard_brightness( _chessboard_brightness );
}

void ofApp::chessboard_projected_changed(bool &_chessboard_projected)
{
  send_chessboard_projected( _chessboard_projected );
}

void ofApp::capture()
{
  ofLog() << "capture";
  calibration.toggle_capture();
}

void ofApp::calibrate()
{
  ofLog() << "calibrate";
  calibration.calibrate();
}

void ofApp::save_calib()
{
  ofLog() << "save calib";
  calibration.save_all( "calib" );
}

void ofApp::save_images()
{
  ofLog() << "save images";
  calibration.save_images("calib/imgs_proj");
}

void ofApp::load_images()
{
  ofLog() << "load images";
  calibration.load_images( "calib/imgs_proj/" );
}

void ofApp::reset_calib()
{
  ofLog() << "reset calib";
  calibration.reset();
}


void ofApp::send_chessboard_brightness( float _chessboard_brightness )
{
  ofxOscMessage m;
  m.setAddress("/chessboard_brightness");
  m.addFloatArg(_chessboard_brightness);
  sender.sendMessage(m);
}

void ofApp::send_chessboard_projected( bool _chessboard_projected )
{
  ofxOscMessage m;
  m.setAddress("/chessboard_projected");
  m.addIntArg(_chessboard_projected?1:0);
  sender.sendMessage(m);
}

void ofApp::update_osc()
{
  while( receiver.hasWaitingMessages() )
  {
    ofxOscMessage m;
    receiver.getNextMessage(&m);

    log_osc_msg( m );

    string addr = m.getAddress();

    if ( ofIsStringInString( addr, chessboard_brightness_osc_addr ) )
    {
      chessboard_brightness = m.getArgAsFloat(0) * 255;
    } 

    else if ( ofIsStringInString( addr, capture_osc_addr ) 
        && m.getArgAsFloat(0) == 1 )
    {
      capture();
    }

    else if ( ofIsStringInString( addr, calibrate_osc_addr ) 
        && m.getArgAsFloat(0) == 1 )
    {
      calibrate();
    }

    else if ( ofIsStringInString( addr, save_calib_osc_addr ) 
        && m.getArgAsFloat(0) == 1 )
    {
      save_calib();
    }

    else if ( ofIsStringInString( addr, save_images_osc_addr ) 
        && m.getArgAsFloat(0) == 1 )
    {
      save_images();
    }

    else if ( ofIsStringInString( addr, load_images_osc_addr ) 
        && m.getArgAsFloat(0) == 1 )
    {
      load_images();
    }

    else if ( ofIsStringInString( addr, chessboard_projected_osc_addr ) )
    {
      chessboard_projected = m.getArgAsFloat(0) == 1;
    }

    else if ( ofIsStringInString( addr, reset_calib_osc_addr ) 
        && m.getArgAsFloat(0) == 1 )
    {
      reset_calib();
    }
  }
}

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


void ofApp::keyPressed(int key)
{
  //if ( key == ' ' ) 
  //{
    //capture();
  //}
}


void ofApp::keyReleased(int key)
{
  if ( key == ' ' ) 
  {
    capture();
  }

  else if ( key == 'c' )
  {
    calibrate();
  }

  else if ( key == 's' )
  {
    save_calib();
  }

  else if ( key == 'i' )
  {
    save_images();
  }

  else if ( key == 'l' )
  {
    load_images();
  }

  else if ( key == 'r' )
  {
    reset_calib();
  }
}


void ofApp::mouseMoved(int x, int y )
{

}


void ofApp::mouseDragged(int x, int y, int button)
{

}


void ofApp::mousePressed(int x, int y, int button)
{

}


void ofApp::mouseReleased(int x, int y, int button)
{

}


void ofApp::windowResized(int w, int h)
{

}


void ofApp::gotMessage(ofMessage msg)
{

}


void ofApp::dragEvent(ofDragInfo dragInfo)
{ 

}

