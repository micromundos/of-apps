#include "params.h"

using namespace std;

void Params::init( string xmlfile, int port ) 
{ 
  receiver.setup( port );

  settings.loadFile( xmlfile );

  int n = settings.getNumTags("control");

  for ( int i = 0; i < n; i++ )
  {
    string k = settings.getValue( 
        "control::label", "undefined", i );

    string v = settings.getValue(
        "control::value", "undefined", i );

    //FIXME
    if ( ofIsStringInString( v, "." ) )
    {
      set( k, ofToFloat( v ) );
    }
    else
    {
      set( k, ofToInt( v ) );
    }
  } 
};

//TODO
//dont iterate on control xml tags
//iterate on params maps (f,i,s) keys
//and update or create new control tags
void Params::save()
{
  int n = settings.getNumTags("control");

  for ( int i = 0; i < n; i++ )
  {
    string k = settings.getValue( 
        "control::label", "undefined", i );

    if ( is_int( k ) )
    {
      settings.setValue(
          "control::value", get_i( k ), i );
    }
    else if ( is_float( k ) )
    {
      settings.setValue(
          "control::value", get_f( k ), i );
    }
  }

  settings.saveFile();
};

//TODO use events
void Params::update() 
{
  while ( receiver.hasWaitingMessages() )
  {
    ofxOscMessage m;
    receiver.getNextMessage( &m );
    update( m ); 
    //log( m ); 
  }
};

void Params::update( ofxOscMessage& m ) 
{
  string addr = m.getAddress();

  if ( ofIsStringInString(
        addr, "/tg/button/save" ) )
  {
    save();
    return;
  }

  if ( ofIsStringInString( addr, "/tg/" ) )
  { 
    string k = ofSplitString(addr,"/").back();
    if ( 
        m.getArgType(0) == OFXOSC_TYPE_INT32)
    {
      set( k, m.getArgAsInt32(0) );
    }
    else if ( 
        m.getArgType(0) == OFXOSC_TYPE_FLOAT)
    {
      set( k, m.getArgAsFloat(0) );
    }
    else if ( 
        m.getArgType(0) == OFXOSC_TYPE_STRING)
    {
      set( k, m.getArgAsString(0) );
    }
  }
};

void Params::log( ofxOscMessage& m ) 
{
  string d = m.getAddress();
  d += ", n args: " + ofToString( m.getNumArgs() );

  for ( int i = 0; i < m.getNumArgs(); i++ )
  {
    d += ", " + ofToString( m.getArgTypeName(i) );

    string val;

    if ( m.getArgType(i) == OFXOSC_TYPE_INT32 )
    {
      val = ofToString( m.getArgAsInt32(i) );
    }
    else if ( m.getArgType(i) == OFXOSC_TYPE_FLOAT )
    {
      val = ofToString( m.getArgAsFloat(i) );
    }
    else if ( m.getArgType(i) == OFXOSC_TYPE_STRING )
    {
      val = m.getArgAsString(i);
    }
    else
    {
      val = "undefined";
    }

    d += ", val: " + val;
  }

  ofLogVerbose() << d;
};

bool Params::has( string k )
{
  return !!f.count(k) 
    || !!i.count(k)
    || !!s.count(k);
};

bool Params::is_int( string k )
{
  return !!i.count(k);
};

bool Params::is_float( string k )
{
  return !!f.count(k);
};

bool Params::is_string( string k )
{
  return !!s.count(k);
};

void Params::set( string k, int v )
{
  if ( ! has( k ) ) return;
  *( i[ k ] ) = v;
};

void Params::set( string k, float v )
{
  if ( ! has( k ) ) return;
  *( f[ k ] ) = v;
};

void Params::set( string k, string v )
{
  if ( ! has( k ) ) return;
  *( s[ k ] ) = v;
}; 

void Params::set( float v )
{
  map<string,float*>::iterator it;
  for ( it = f.begin(); it != f.end(); ++it ) 
    *(it->second) = v;
}; 

void Params::set( int v )
{
  map<string,int*>::iterator it;
  for ( it = i.begin(); it != i.end(); ++it ) 
    *(it->second) = v;
};

void Params::set( string v )
{
  map<string,string*>::iterator it;
  for ( it = s.begin(); it != s.end(); ++it ) 
    *(it->second) = v;
};

int Params::get_i( string k )
{
  if ( ! is_int( k ) ) return -1;
  return *( i[ k ] );
};

float Params::get_f( string k )
{
  if ( ! is_float( k ) ) return -1;
  return *( f[ k ] );
};

string Params::get_s( string k )
{
  if ( ! is_string( k ) ) return "undefined";
  return *( s[ k ] );
}; 

