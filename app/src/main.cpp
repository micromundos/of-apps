#include "ofMain.h"
#include "ofApp.h"
//#include "ofAppGlutWindow.h"

int main()
{
  //ofAppGlutWindow window;
  //ofSetupOpenGL(&window,100,100,OF_WINDOW);
  ofSetupOpenGL( 
      100,100, //tmp
      //OF_WINDOW );
    OF_WINDOW_UNDECORATED );
  ofRunApp(new ofApp());
}

