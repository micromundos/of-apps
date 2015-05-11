#include "ofMain.h"
#include "data_path.h"
#include "ofApp.h"

int main()
{
  ofSetDataPathRoot( __data_path__ );
	ofSetupOpenGL(500,1000,OF_WINDOW);
	ofRunApp(new ofApp());
}

