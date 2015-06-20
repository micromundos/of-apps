#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class DepthHoleFillerComponent : public ECScomponent 
{
  public:

    ofFloatImage output;
    bool enabled; //use_history || use_closing || use_contour

    ofParameter<bool>  render;
    ofParameter<bool>  use_history;
    ofParameter<bool>  use_closing;
    ofParameter<bool>  use_contour;

    ofParameter<float>  history;
    ofParameter<float>  closing_passes;
    ofParameter<float>  contour_max_area;
    ofParameter<float>  contour_max_holes;
    ofParameter<float>  contour_delta;
    ofParameter<float>  contour_fill_width;

    DepthHoleFillerComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      param( render, "render" );

      param( use_contour, "use_contour" );
      use_closing = false;
      //param( use_closing, "use_closing" );
      use_history = false;
      //param( use_history, "use_history" );

      history = 0;
      //param( history, "history" );
      closing_passes = 0;
      //param( closing_passes, "closing_passes" );

      param( contour_max_area, "contour_max_area" );
      param( contour_max_holes, "contour_max_holes" );
      param( contour_delta, "contour_delta" );
      param( contour_fill_width, "contour_fill_width" );
    };

};

