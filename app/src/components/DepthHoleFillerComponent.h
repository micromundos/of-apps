#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class DepthHoleFillerComponent : public ECScomponent 
{
  public:

    //float _data; //to get value from config.json or live in code only
    //ofParameter<float> _param; //to get value from a sender via osc
    ofParameter<bool>  render;
    ofParameter<bool>  use_history;
    ofParameter<bool>  use_closing;
    ofParameter<bool>  use_contour;
  
    ofParameter<float>  depth_history;
    ofParameter<float>  passes;
    ofParameter<float>  contour_max_area;
    ofParameter<float>  contour_max_holes;
    ofParameter<float>  contour_delta;
    ofParameter<float>  contour_fill_width;
 
  
    DepthHoleFillerComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);
      param( render, "render" );
      param( use_history, "use_history" );
      param( use_closing, "use_closing" );
      param( use_contour, "use_contour" );
      param( depth_history, "depth_history" );
      param( passes, "passes" );
      param( contour_max_area, "contour_max_area" );
      param( contour_max_holes, "contour_max_holes" );
      param( contour_delta, "contour_delta" );
      param( contour_fill_width, "contour_fill_width" );
      //_data = d.get("_data",1.0f).asFloat();
      //param(_param, "_param");
    };

};

