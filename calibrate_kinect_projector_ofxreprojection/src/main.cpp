#include "ofApp.h"
//#include "ofAppGLFWWindow.h"

int main() 
{
  //ofAppGLFWWindow window;
  ofSetupOpenGL(
      //&window, 
      100, 100, 
      OF_WINDOW_UNDECORATED );
      //OF_WINDOW);
  ofRunApp(new ofApp());
}
