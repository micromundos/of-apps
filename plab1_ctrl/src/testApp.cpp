#include "testApp.h"


int pcant;
float drag;

float krep;
float katt;
//float springk;
//float springdamp;

float psize;
float pbirthrandarea;
float pradioatt;
float pradiorep;
float pradioattmin;

float pmaxforce;
float pmaxspeed;
float pflowmult;

float dpslope;
float dpnear, dpfar;
int dpleft, dpright;
int dptop, dpbottom;

bool pstratmesh;
bool pstratflowfield;

bool rendertexkinect;
bool rendertexkinecthue;
bool rendertexcv;
bool rendertexblobs;

bool rendercv;
bool renderpradioatt;
bool renderlinks;
bool renderflowfield;

bool debug;
bool wireframe;
bool fullscreen;
bool projector;


void testApp::setup(){

  ofSetWindowPosition(0,0);

  //string ddOptions[] = {"Oranges", "Bananas", "Apples", "Kiwis", "Mangoes"};

  host = "127.0.0.1"; // change via xml
  port = 4444; // change via xml


  // gui settings
  settings.loadSettings("settings.xml", true, true); // savefile, default font, use mouse
  //settings.loadBackground("guiBg.png");    
  //settings.autoDraw();


  // add any random properties to be saved - good for configs, once set can only be changed via xml
  settings.setConstant("host", &host);
  settings.setConstant("port", &port);
  //settings.setVariable("host", &host); //can be changed
  //settings.setVariable("port", &port); // can be changed


  // enable osc
  settings.setupSendOSC(host, port); 


  // add controls    

  settings.addText("ofxTouchGUI - " + host + ":" + ofToString(port));

  settings.addText("particles physics");

  settings.addSlider("pcant", &pcant, 0, 1000);
  settings.addSlider("drag", &drag, 0.0f, 0.3f);

  //settings.addSlider("springk", &springk, 0.0f, 1.0f);
  //settings.addSlider("springdamp", &springdamp, 0.0f, 1.0f);
  settings.addSlider("krep", &krep, 0.0f, -100.0f);
  settings.addSlider("katt", &katt, 0.0f, 100.0f);

  settings.addSlider("psize", &psize, 0.0f, 10.0f);
  settings.addSlider("pbirthrandarea", &pbirthrandarea, 0.0f, 100.0f);
  settings.addSlider("pradioatt", &pradioatt, 0.0f, 100.0f);
  settings.addSlider("pradiorep", &pradiorep, 0.0f, 100.0f);
  settings.addSlider("pradioattmin", &pradioattmin, 0.0f, 100.0f);
  settings.addSlider("pmaxforce", &pmaxforce, 0.0f, 10.0f);
  settings.addSlider("pmaxspeed", &pmaxspeed, 0.0f, 10.0f);
  settings.addSlider("pflowmult", &pflowmult, 0.0f, 10.0f);

  settings.addText("particles stategies");
  settings.addToggleButton("pstratmesh", &pstratmesh);
  settings.addToggleButton("pstratflowfield", &pstratflowfield);

  settings.addText("depth process");
  settings.addSlider("dpslope", &dpslope, 0.0f, 500.0f);
  settings.addSlider("dpnear", &dpnear, 0.0f, 5000.0f);
  settings.addSlider("dpfar", &dpfar, 0.0f, 5000.0f);
  settings.addSlider("dpleft", &dpleft, 0, 640);
  settings.addSlider("dpright", &dpright, 0, 640);
  settings.addSlider("dptop", &dptop, 0, 480);
  settings.addSlider("dpbottom", &dpbottom, 0, 480);

  settings.addText("render");
  settings.addToggleButton("rendertexkinect", &rendertexkinect);
  settings.addToggleButton("rendertexkinecthue", &rendertexkinecthue);
  settings.addToggleButton("rendertexcv", &rendertexcv);
  settings.addToggleButton("rendertexblobs", &rendertexblobs);

  settings.addToggleButton("rendercv", &rendercv);
  settings.addToggleButton("renderpradioatt", &renderpradioatt);
  settings.addToggleButton("renderlinks", &renderlinks);
  settings.addToggleButton("renderflowfield", &renderflowfield);

  settings.addToggleButton("debug", &debug);
  settings.addToggleButton("wireframe", &wireframe);
  settings.addToggleButton("fullscreen", &fullscreen);
  settings.addToggleButton("projector", &projector);

  ofxTouchGUIButton* sendBtn = settings.addButton("SEND"); 
  ofAddListener(sendBtn->onChangedEvent, this, &testApp::onButtonPressed);
  ofxTouchGUIButton* saveBtn = settings.addButton("SAVE"); 
  ofAddListener(saveBtn->onChangedEvent, this, &testApp::onButtonPressed);
  //ofxTouchGUIButton* reload_btn = settings.addButton("RELOAD");
  //ofAddListener(reload_btn->onChangedEvent, this, &testApp::onButtonPressed);
  //ofxTouchGUIButton* resetBtn = settings.addButton("RESET");
  //ofAddListener(resetBtn->onChangedEvent, this, &testApp::onButtonPressed);

  //settings.nextColumn(); // jump to a new column

}


void testApp::onButtonPressed(const void* sender, string &buttonLabel) {

  // could use the pointer to button that was pressed? eg. 
  ofxTouchGUIButton * button = (ofxTouchGUIButton*)sender;
  cout << buttonLabel << " - " << button->getValue() << endl;

  // or just use the label as the identifier
  if ( buttonLabel == "SAVE" ) 
  {
    settings.saveSettings();
    sendAll();
  }
  //else if ( buttonLabel == "RELOAD" ) 
  //{ 
    //settings.loadSettings("settings.xml", true, true); // savefile, default font, use mouse
  //}
  else if ( buttonLabel == "RESET" ) 
  { 
    settings.resetDefaultValues();
  }
  else if ( buttonLabel == "SEND" ) 
  { 
    sendAll();
  }

}

void testApp::sendAll() {

  ofxXmlSettings XML = settings.XML;

  // loop through the xml and udpate the values from the gui
  int numSavedControllers = XML.getNumTags("control");

  for(int i = 0; i < numSavedControllers; i++){

    XML.pushTag("control", i);
    string controlLabel = XML.getValue("label", "", 0);
    string controlType = XML.getValue("type", "", 0); // get the type value or ""
    //string controlItemId = XML.getValue("itemid", "", 0);
    if(controlType == TOGGLE_TYPE) {

      //ofxTouchGUIBase* controller = getItemByLabel(controlLabel);
      //const ofxTouchGUIToggleButton* controller = (const ofxTouchGUIToggleButton*)getItemById(controlItemId);
      ofxTouchGUIToggleButton* controller = (ofxTouchGUIToggleButton*)(settings.getItemByLabelAndType)(controlLabel,controlType);
      if(controller) {
        //XML.setValue("value", *controller->toggleVal, 0);
        controller->sendOSC( *controller->toggleVal );
      }
    }        
    else if(controlType == SLIDER_TYPE) {

      //const ofxTouchGUISlider* controller = (const ofxTouchGUISlider*)getItemById(controlItemId);
      ofxTouchGUISlider* controller = (ofxTouchGUISlider*)(settings.getItemByLabelAndType)(controlLabel,controlType);
      if(controller) {
        if(controller->useInteger == true) {
          //XML.setValue("value", *controller->intVal, 0);
          controller->sendOSC( *controller->intVal );
        }
        else {
          //XML.setValue("value", *controller->val, 0);
          controller->sendOSC( *controller->val );
        }   
      }

    }
    else if(controlType == DROPDOWN_TYPE) {

      //const ofxTouchGUIDropDown* controller = (const ofxTouchGUIDropDown*)getItemById(controlItemId);
      ofxTouchGUIDropDown* controller = (ofxTouchGUIDropDown*)(settings.getItemByLabelAndType)(controlLabel,controlType);
      if(controller) {
        //XML.setValue("value", *controller->selectId, 0);
        controller->sendOSC( *controller->selectId );
      }            
    }
    else if(controlType == TEXTINPUT_TYPE) {

      ofxTouchGUITextInput* controller = (ofxTouchGUITextInput*)(settings.getItemByLabelAndType)(controlLabel,controlType);
      if(controller) {
        //XML.setValue("value", *controller->input, 0);
        controller->sendOSC( *controller->input );
      }            
    }
    /*
     * can't send NameValuePair 
     * set up by settings.setVariable
     */
    //else if(controlType == VAR_TYPE) {
      ////NameValuePair nvp;
      ////= {varName, (void*)regVar};
      ////varItems.push_back(nvp);
      //const NameValuePair* var = (const NameValuePair*)settings.getVarByLabel(controlLabel);
      //if(var) {

        //if(var->type == _STRING) {
          //XML.setValue("value", *(string*)var->value, 0);
        //} else if(var->type == _INT) {
          //XML.setValue("value", *(int*)var->value, 0);
        //} else if(var->type == _FLOAT) {
          //XML.setValue("value", *(float*)var->value, 0);
        //} else if(var->type == _BOOL) {
          //XML.setValue("value", *(bool*)var->value, 0);
        //}
      //}  
    //}

    // note fixed vars/constants aren't saved, they are set once on creation
    // need to add more generic 'vars/addVar' property for a changing var - done!

    XML.popTag();
  }

  cout << "Sending all data " << endl;

}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

  ofBackground( 255, 150, 0 );
  ofEnableAlphaBlending();

  // manual ui drawing
  settings.draw(); 

  ofDisableAlphaBlending();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){


}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

  if(key == ' ') settings.toggleDisplay();
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
