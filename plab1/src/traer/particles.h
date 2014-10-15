#pragma once

#include "ofxTraerPhysics.h"
#include "ofVboMesh.h"
#include "params.h"
#include "coord.h"
#include "flowfield.h"
#include "dp_cv.h"
#include "dp_blobs.h"

using namespace traer::physics;

class Particles;

namespace strat
{

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

  virtual ~Strategy()
  {
    params = NULL;
    particles = NULL;
  };

  virtual void update(){};
  virtual void render(){};

protected:

  Params* params;
  Particles* particles;
};

class Stack
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

class DepthFlowField : public Strategy
{
public:

  DepthFlowField( 
      Params* params, 
      Particles* particles,
      FlowField* ff 
      )
    : Strategy( params, particles )
  {
    this->ff = ff;
  };

  ~DepthFlowField()
  {
    ff = NULL;
  };

  void update();

private:

  FlowField* ff;
};

class Mesh : public Strategy
{
public:

  Mesh( 
      Params* params, 
      Particles* particles 
      )
    : Strategy( params, particles )
  {};

  ~Mesh(){};

  void update();
  void render();
};

class Birth : public Strategy
{
public:

  Birth( 
      Params* params, 
      Particles* particles, 
      DepthProcess::blobs* blobs,
      coord cv2tex
      )
    : Strategy( params, particles )
  {
    this->blobs = blobs;
    this->cv2tex = cv2tex;
  };

  ~Birth()
  {
    blobs = NULL;
  };

  void update();

private:

  DepthProcess::blobs* blobs;
  coord cv2tex;
};

}; //namespace strat

/*
 * forces
 */

class Flow : public Force, public Parametrizable
{
public:

  Flow( 
      Params* params, 
      Particle* p, 
      ofVec2f& flow )
  {
    Parametrizable::init( params );
    this->p = p;
    this->flow = flow;
  };
  ~Flow() 
  {
    params = NULL;
    p = NULL;
  };

  void apply()
  {
    float maxforce = params->pmaxforce; 
    float maxspeed = params->pmaxspeed;
    float mult = params->pflowmult;

    ofVec3f steer = ( flow * maxspeed ) - p->velocity;

    steer.x = CLAMP( steer.x, 
        -maxforce, maxforce );
    
    steer.y = CLAMP( steer.y, 
        -maxforce, maxforce );
    
    p->force += steer * mult;
  };

  Particle* p;
  ofVec2f flow;

  void turnOn() {};
	void turnOff() {};
	bool isOn() const { return true; };
	bool isOff() const { return false; };

};

class Seek : public Force, public Parametrizable
{
public:

  Seek( 
      Params* params, 
      Particle* p, 
      ofVec2f& target,
      float mult = 1. )
  {
    Parametrizable::init( params );
    this->p = p;
    this->target = target;
    this->mult = mult;
  };
  ~Seek() 
  {
    params = NULL;
    p = NULL;
  };

  void apply()
  {
    float maxforce = params->pmaxforce; 
    float maxspeed = params->pmaxspeed;

    ofVec3f steer;
    ofVec3f desired = ofVec3f(
        target - p->position );

    //desired.normalize();
    float d = desired.length();
    if ( d > 0 )
    {
      desired /= d; //normalize
      desired *= maxspeed;

      steer = desired - p->velocity;
      steer.x = CLAMP( steer.x, 
          -maxforce, maxforce );
      steer.y = CLAMP( steer.y, 
          -maxforce, maxforce );
    }

    p->force += steer;
  };

  Particle* p;
  ofVec2f target;
  float mult;

  void turnOn() {};
	void turnOff() {};
	bool isOn() const { return true; };
	bool isOff() const { return false; };

};

class Particles : public Parametrizable
{
public:

  Particles(){};
  ~Particles(){};

  void init( 
      Params* params, 
      coord tex2depth );

  void update();
  void render();   

  void update( uint16_t *mm_depth_pix );

  void add_strat( string id, strat::Strategy* s )
  {
    strats.add( id, s );
  };

  void remove_strat( string id )
  {
    strats.remove( id );
  };

  void clear_forces();
  void clear_springs();
  void clear_attractions();

  void add( int amt, int x, int y );

  ParticleSystem* ps;

private:

  ofVboMesh bodies;

  vector<Particle*> to_remove;

  void depth_bounds( uint16_t *mm_depth_pix );
  void tex_bounds();
  void render_bodies(); 

  coord tex2depth;

  void remove( int amt );

  strat::Stack strats;

  uint16_t get_depth(
      ofVec3f& ploc, 
      uint16_t *depth_pix );
};

