#include "ofApp.h"

int projector_width = 1024;
int projector_height = 768;
int monitor_width;
int monitor_height;

bool rendererInited = false;

void ofApp::setup()
{
  monitor_width = ofGetScreenWidth();
  monitor_height = ofGetScreenHeight();

  ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);

  ofSetWindowPosition( 0, 0 );
  ofSetWindowShape( 
      monitor_width + projector_width,
      projector_height );

	kinect.init();
	kinect.setUseTexture(false);
	kinect.open();

  dataset.loadFile("ofxReprojection.xml");

	calibration.init( &kinect, &dataset );
	calibration.enableKeys();
	calibration.enableChessboardMouseControl();

	rendererInited = false;
}

void ofApp::update()
{

	kinect.update();

  if ( !calibration.isFinalized() ) 
  {
		calibration.update();
	}

	if ( calibration.isFinalized() && !rendererInited ) 
  {
		renderer.setDrawMethod(OFXREPROJECTIONRENDERER_2DDRAWMETHOD_POINTS);
		renderer.init( &kinect );
		renderer.setDrawArea(
        monitor_width, 0,
        projector_width, projector_height );
		renderer.setProjectionMatrix( dataset.getMatrix() );
		rendererInited = true;
	}

	if ( calibration.isFinalized() && rendererInited ) 
  {
		renderer.update();
	}
}

void ofApp::draw(){

	if ( !calibration.isFinalized() ) 
  {
		calibration.drawStatusScreen( 0,0, monitor_width, monitor_height );
		calibration.drawChessboard(
        monitor_width, 0,
        projector_width, projector_height );
	}

	if ( calibration.isFinalized() && rendererInited ) 
  {
		renderer.drawHueDepthImage();
	}
}

void ofApp::keyPressed(int key){}

void ofApp::exit(){}


