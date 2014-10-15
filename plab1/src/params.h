#pragma once

#include "ofxXmlSettings.h"
#include "ofxOsc.h"

using namespace std;

class Params 
{
public:

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

  int pstratmesh;
  int pstratflowfield;

  int rendertexkinect;
  int rendertexkinecthue;
  int rendertexcv;
  int rendertexblobs;

  int rendercv;
  int renderpradioatt;
  int renderlinks;
  int renderflowfield;

  int debug;
  int wireframe;
  int fullscreen;
  int projector;

  Params()
  {
    i["pcant"] = &pcant;
    f["drag"] = &drag;

    f["krep"] = &krep;
    f["katt"] = &katt;
    //f["springk"] = &springk;
    //f["springdamp"] = &springdamp;

    f["psize"] = &psize;
    f["pbirthrandarea"] = &pbirthrandarea;
    f["pradioatt"] = &pradioatt;
    f["pradiorep"] = &pradiorep;
    f["pradioattmin"] = &pradioattmin;

    f["pmaxforce"] = &pmaxforce;
    f["pmaxspeed"] = &pmaxspeed;
    f["pflowmult"] = &pflowmult;

    f["dpslope"] = &dpslope;
    f["dpnear"] = &dpnear;
    f["dpfar"] = &dpfar;
    i["dpleft"] = &dpleft;
    i["dpright"] = &dpright;
    i["dptop"] = &dptop;
    i["dpbottom"] = &dpbottom;

    i["pstratmesh"] = &pstratmesh;
    i["pstratflowfield"] = &pstratflowfield;

    i["rendertexkinect"] = &rendertexkinect;
    i["rendertexkinecthue"] = &rendertexkinecthue;
    i["rendertexcv"] = &rendertexcv;
    i["rendertexblobs"] = &rendertexblobs;

    i["rendercv"] = &rendercv;
    i["renderpradioatt"] = &renderpradioatt;
    i["renderlinks"] = &renderlinks;
    i["renderflowfield"] = &renderflowfield;

    i["debug"] = &debug;
    i["wireframe"] = &wireframe;
    i["fullscreen"] = &fullscreen;
    i["projector"] = &projector;

    set( 0 );
    set( 0.0f ) ;
    set( "undefined" );
    
  };

  ~Params()
  {
  };

  void init( string xmlfile, int port );
  void save();
  void update();

private:

  ofxXmlSettings settings;
  ofxOscReceiver receiver;

  map<string,float*> f;
  map<string,int*> i;
  map<string,string*> s; 

  void update( ofxOscMessage& m );
  void log( ofxOscMessage& m );

  bool has( string k );
  bool is_int( string k );
  bool is_float( string k );
  bool is_string( string k );

  void set( string k, float v );
  void set( string k, int v );
  void set( string k, string v );

  void set( float v );
  void set( int v );
  void set( string v );

  float get_f( string k );
  int get_i( string k );
  string get_s( string k );

};

class Parametrizable
{
  public:

    Parametrizable(){};
    virtual ~Parametrizable()
    {
      params = NULL;
    };

    void init( Params* params )
    {
      this->params = params;
    };

  protected:

    Params* params;
};

