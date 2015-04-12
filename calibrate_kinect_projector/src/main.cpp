#include "ofMain.h"
#include "ofApp.h"

int main()
{ 
  ofSetupOpenGL(
      100,100, //tmp
      OF_WINDOW_UNDECORATED );
      //OF_WINDOW );
  ofRunApp(new ofApp());
}
