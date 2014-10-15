#pragma once

#include "params.h"
#include "particle.h"
#include "flowfield.h"
#include "ofVboMesh.h"

class Particles;

class Strategy
{
public:

  Strategy( 
      Params* params, 
      Particles* particles )
  {
    this->params = params;
    this->particles = particles;
  };

  virtual ~Strategy(){};

  virtual void update(){};
  virtual void render(){};

protected:

  Params* params;
  Particles* particles;
};

class FlowFieldStrategy : public Strategy
{
public:

  FlowFieldStrategy( 
      Params* params, 
      Particles* particles,
      FlowField* ff )
    : Strategy( params, particles )
  {
    this->ff = ff;
  };

  ~FlowFieldStrategy(){};

  void update();
  void render();

  FlowField* ff;
 
};

class MeshStrategy : public Strategy
{
public:

  MeshStrategy( 
      Params* params, 
      Particles* particles )
    : Strategy( params, particles )
  {};

  ~MeshStrategy(){};

  void update();
  void render();
};

class StrategyStack
{
public:

  void update()
  {
    map<string,Strategy*>::iterator it;
    for ( it = strats.begin(); 
        it != strats.end(); 
        ++it ) 
      it->second->update();
  };

  void render()
  {
    map<string,Strategy*>::iterator it;
    for ( it = strats.begin(); 
        it != strats.end(); 
        ++it ) 
      it->second->render();
  };

  void add( string id, Strategy* s )
  {
    strats[ id ] = s;
  };

  void remove( string id )
  {
    strats.erase( id );
  };

private:

  map<string,Strategy*> strats;

};


class Particles : public Parametrizable
{
public:

  Particles(){};
  ~Particles(){};

  void init( 
      Params* params, 
      int width, int height,
      int depth_width, int depth_height );

  void update( uint16_t *mm_depth_pix );
  void update();
  void render();   

  void add_strat( string id, Strategy* s )
  {
    strats.add( id, s );
  };

  void remove_strat( string id )
  {
    strats.remove( id );
  };

  vector<Particle*> ps;

private:

  ofVboMesh bodies;

  void render_bodies(); 

  int width, height;
  int depth_width, depth_height;

  void add( int amt );
  void remove( int amt );

  StrategyStack strats;

  uint16_t get_depth(
      ofVec2f& ploc, 
      uint16_t *depth_pix );
};

