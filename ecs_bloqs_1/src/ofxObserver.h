
//TODO

#pragma once

#include "ofMain.h"

class ofxObserver
{
  public:
    ofxObserver(){};
    ~ofxObserver(){};

    template <typename ArgumentsType>
    ofEvent<ArgumentsType> event( string id )
    {};

    template <typename ArgumentsType, class ListenerClass>
    void on( string id, ListenerClass* listener, void (ListenerClass::*listenerMethod)(ArgumentsType&) )
    {
      ofAddListener( event(id), listener, listenerMethod );
    };

    template <typename ArgumentsType, class ListenerClass>
    void off( string id, ListenerClass* listener, void (ListenerClass::*listenerMethod)(ArgumentsType&) )
    {
      ofRemoveListener( event(id), listener, listenerMethod );
    }; 

};
