#pragma once

#include <Artemis/Artemis.h>
#include "ofMain.h"
#include "ofxJSON.h"

class ECScomponent : public artemis::Component
{

  public:

    ECScomponent(string _id) : id(_id)
    {};

    virtual ~ECScomponent() 
    {
      _params = NULL; 
    };

    string id;

    virtual void init( Json::Value d, ofParameterGroup* p )
    {
      _data = d;
      _params = p;
    }; 

  protected:

    Json::Value _data;
    ofParameterGroup* _params;

    //TODO shared code !!! Params.h->parse()
    template<typename T>
    void param(ofParameter<T>& _param, string name) 
    {
      Json::Value jval = _data[name];
      T val;

      if ( jval.isArray() )
      {

        if (jval.size() != 3 )
        {
          ofLogError("ECScomponent") << "set data from config for component --" << name << "-- has an array value with (" << jval.size() << ") values, and it needs exactly (3): [value, min, max]";
          return;
        }

        val = get_value<T>( jval[0] );
        T min = get_value<T>( jval[1] );
        T max = get_value<T>( jval[2] );

        _param.setMin(min);
        _param.setMax(max);
      }
      else
      {
        val = get_value<T>( jval );
      }

      if ( _params == NULL )
      {
        ofLogError("ECScomponent") << "set data from config for component --" << name << "-- ofParameterGroup is not initialized!";
        return;
      }

      _params->add( _param.set( name, val ) );
    }; 

    template<typename T>
    T get_value( Json::Value d ) {};

};

