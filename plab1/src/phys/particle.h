#pragma once

#include "params.h"

class Particle : public Parametrizable
{
  public:

    Particle( 
        Params* params, 
        float x, float y,
        int width, int height )
    {
      Parametrizable::init( params );
      this->position.set( x, y );
      this->width = width;
      this->height = height;
    };
    ~Particle(){};

    ofVec2f position, velocity, force;

    void flowfield( ofVec2f& flow )
    {
      float maxforce = params->pmaxforce; 
      float maxspeed = params->pmaxspeed;
      float mult = params->pflowmult;

      ofVec3f steer = ( flow * maxspeed ) - velocity;

      steer.x = CLAMP( steer.x, 
          -maxforce, maxforce );

      steer.y = CLAMP( steer.y, 
          -maxforce, maxforce );

      force += steer * mult;
    };

    void seek( ofVec2f& target, float mult = 1. )
    {
      float maxforce = params->pmaxforce; 
      float maxspeed = params->pmaxspeed;

      ofVec3f steer;
      ofVec3f desired = ofVec3f(
          target - position );

      //desired.normalize();
      float d = desired.length();
      if ( d > 0 )
      {
        desired /= d; //normalize
        desired *= maxspeed;

        steer = desired - velocity;
        steer.x = CLAMP( steer.x, 
            -maxforce, maxforce );
        steer.y = CLAMP( steer.y, 
            -maxforce, maxforce );
      }

      force += steer * mult;
    };

    void update()
    {
      float maxspeed = params->pmaxspeed;

      velocity += force;

      velocity.x = CLAMP(
          velocity.x, -maxspeed, maxspeed); 
      velocity.y = CLAMP(
          velocity.y, -maxspeed, maxspeed);

      position += velocity;	

      force.set( 0, 0 );
    };

    void bounds()
    {
      if ( position.x < 0 )
      {
        position.x = 0;
        velocity.x *= -1;
      }
      else if ( position.x > width )
      {
        position.x = width;
        velocity.x *= -1;
      }

      if ( position.y < 0 )
      {
        position.y = 0;
        velocity.y *= -1;
      }
      else if ( position.y > height )
      {
        position.y = height;
        velocity.y *= -1;
      }
    };

  private:

    int width, height;

    //void seek(ofPoint target, float mult = 1.);
    //ofVec3f steer( ofPoint target, float *slowdown = NULL );

};

