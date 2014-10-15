#include "testApp.h"

void testApp::setup() 
{
  ofSetLogLevel(OF_LOG_VERBOSE);
  ofSetFrameRate(60);
  ofBackground(50);

  update_window( true );


  _near_mm = 500.;
  _far_mm = 4000.;

  kinect.setRegistration(false);
  //kinect.setDepthMode( FREENECT_DEPTH_11BIT );
  kinect.setDepthClipping(_near_mm, _far_mm); //mm (50cm - 4mt)
  kinect.enableDepthNearValueWhite(true);
  // ir:false, rgb:false, texture:true
  kinect.init(false, false, true);
  kinect.open();


  string cfg = ofToDataPath("camara_lucida/config.xml");

  depthmap = new cml::Depthmap_freenect();
  cml = new cml::CamaraLucida( cfg, depthmap );

  ofAddListener( cml->render_texture, 
      this, &testApp::render_texture );

  ofAddListener( cml->render_3d, 
      this, &testApp::render_3d );

  ofAddListener( cml->render_2d, 
      this, &testApp::render_2d );


  params.init( "settings.xml", 4444 );

  coord tex2depth(
      cml->tex_width(), 
      cml->tex_height(),
      cml->depth_width(),
      cml->depth_height()
      ); 

  coord cv2depth(
      160, 120,
      cml->depth_width(), 
      cml->depth_height()
      );

  coord cv2tex(
      160, 120,
      cml->tex_width(), 
      cml->tex_height()
      );

  coord ff2depth(
      100, 100,
      cml->depth_width(), 
      cml->depth_height()
      );

  coord tex2ff(
      cml->tex_width(), 
      cml->tex_height(),
      100, 100
      );

  cv.init( &params, cv2depth ); 

  gpu.init(
      &params,
      cml->depth_width(),
      cml->depth_height()
      ); 

  ff.init( &params, 
      ff2depth, tex2ff, tex2depth ); 

  ps.init( &params, tex2depth ); 

  ps.add_strat( 
      "pstratbirth", 
      new strat::Birth( 
        &params, &ps, 
        &blobs, 
        cv2tex ) );

  ps.add_strat( 
      "pstratmesh", 
      new strat::Mesh( 
        &params, &ps ) );

  ps.add_strat( 
      "pstratflowfield", 
      new strat::DepthFlowField( 
        &params, &ps, &ff ) );

}

void testApp::update() 
{	
  params.update();

  cml->debug( params.debug );
  cml->wireframe( params.wireframe );
  update_window();

  kinect.update();

  if ( ! kinect.isFrameNew() )
  {
    ps.update(); 
    return;
  }

  uint16_t *mm = kinect.getRawDepthPixels();

  depthmap->update( mm );

  //ofTexture& depth_ftex = depthmap->get_float_tex_ref( mm, _near_mm, _far_mm );

  cv.update( mm );

  //gpu.update( mm, depth_ftex );

  blobs.update( cv.image() );

  ff.update( mm );
  //ff.update( mm, depth_ftex );

  ps.update( mm ); 
}

void testApp::draw() 
{     
  cml->render(); 
}

void testApp::render_texture(ofEventArgs &args)
{
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float w = cml->tex_width();
  float h = cml->tex_height();

  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glColor3f(1, 1, 1);

  if ( params.rendertexkinect )
    kinect.drawDepth( 0, 0, w, h );

  if ( params.rendertexkinecthue )
    depthmap->get_hue_tex_ref(
        kinect.getRawDepthPixels() )
      .draw( 0, 0, w, h );

  if ( params.rendertexcv )
    cv.draw( 0, 0, w, h );

  if ( params.rendertexblobs )
    blobs.draw( 0, 0, w, h );

  //glColor3f(1, 1, 0);
  //ofCircle(800, 200, 60);

  ps.render(); 
  ff.render();
}

void testApp::render_3d(ofEventArgs &args)
{
  //glScalef( 1., -1., 1. );	
  //glTranslatef( -0.3, 0.3, 1. );
  //glColor3f(1, 1, 1);
  //glutWireTeapot(0.1);
}

void testApp::render_2d(ofEventArgs &args)
{
  gpu.draw( 320, 0, 320, 240 );
  if ( params.rendercv )
  {
    cv.draw( 0, 0, 320, 240 );
    blobs.draw( 0, 0, 320, 240 );
  }
  //ofSetColor(255, 255, 255);
  //kinect.drawDepth(0, 0, 200, 150);
  //ofDrawBitmapString("press h for help",10,10);
}

void testApp::exit() 
{
  ofLog(OF_LOG_VERBOSE, "exit!");

  ofRemoveListener( cml->render_texture, 
      this, &testApp::render_texture );

  ofRemoveListener( cml->render_3d, 
      this, &testApp::render_3d );

  ofRemoveListener( cml->render_2d, 
      this, &testApp::render_2d );

  cml->dispose();
  depthmap->dispose();

  kinect.close();
}

void testApp::keyPressed(int key) 
{
  switch (key) {

    case 'w':
      cml->wireframe( ! cml->wireframe() );
      break;

    //case 'e':
      //kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
      //break;

    case 'f':
      ofToggleFullscreen();
      break;

    case 'p':
      ofSetWindowPosition( ofGetWindowPositionX() == 0 ? 1440 : 0, 0 );
      break;
  }
}

void testApp::update_window( bool init )
{ 
  int monitor_width = 1440; 

  if ( init )
  {
    ofSetWindowPosition( monitor_width - ofGetWindowWidth(), 0 );
    return;
  }

  // projector
  bool projecting = ofGetWindowPositionX() == monitor_width;
  if ( params.projector && ! projecting )
    ofSetWindowPosition( monitor_width, 0 );
  else if ( ! params.projector && projecting )
    ofSetWindowPosition( monitor_width - ofGetWindowWidth(), 0 );

  // fullscreen
  int mode = ofGetWindowMode();
  if ( params.fullscreen 
      && mode != OF_FULLSCREEN )
    ofSetFullscreen( true );
  else if ( ! params.fullscreen 
      && mode == OF_FULLSCREEN )
    ofSetFullscreen( false );
}  

void testApp::mouseDragged(int x, int y, int button)
{}

void testApp::mousePressed(int x, int y, int button)
{}

void testApp::mouseReleased(int x, int y, int button)
{}

void testApp::windowResized(int w, int h)
{}

