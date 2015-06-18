#pragma once

#include "ofMain.h"
#include <Artemis/Artemis.h>
#include "ofxECS.h"

class ParticleSystemComponent : public ECScomponent
{
  public:

    float max_particles;
    float radius;

    ofParameter<float> max_speed;
    ofParameter<float> lifetime;
    ofParameter<float> render_size;

    ParticleSystemComponent(string _id) : ECScomponent(_id) {};

    virtual void init( string e_id, Json::Value d, ECSparams* p )
    {
      ECScomponent::init(e_id,d,p);

      max_particles = d["max_particles"].asFloat();
      radius = d["radius"].asFloat();

      param( max_speed, "max_speed" );
      param( lifetime, "lifetime" );
      param( render_size, "render_size" );
    };
};

