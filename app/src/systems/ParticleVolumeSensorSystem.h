#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

using namespace artemis;

class ParticleVolumeSensorSystem : public ECSsystem 
{ 

  public:

    ParticleVolumeSensorSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<ParticleVolumeSensorComponent>();
      addComponentType<BloqComponent>();
    };

    virtual void initialize() 
    {
      particle_volume_sensor_m.init( *world );
      bloq_m.init( *world );
    };

    //virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    //{
      //EntityProcessingSystem::processEntities(bag);
      //int len = bag.getCount();
      //for ( int i = 0; i < len; i++ )
        //processEntity( *bag.get(i) );
    //};

    //virtual void renderEntities( ImmutableBag<Entity*>& bag ) 
    //{
      //EntityProcessingSystem::renderEntities(bag);
    //};

    virtual void removed(Entity &e) 
    {};

    virtual void added(Entity &e) 
    {
      ParticleVolumeSensorComponent* pvs_data = particle_volume_sensor_m.get(e);
      pvs_data->volume = 0.0;
      pvs_data->volume_n = 0.0;
      pvs_data->volume_i = 0.0;
      pvs_data->volume_prev = 0.0;
    }; 

    virtual void processEntity(Entity &e) 
    {
      TS_START("ParticleVolumeSensorSystem"); 

      RenderComponent* render_data = component<RenderComponent>("output");

      ParticleVolumeSensorComponent* pvs_data = particle_volume_sensor_m.get(e);

      pvs_data->volume_prev = pvs_data->volume;
      pvs_data->volume = 0.0;

      Bloq* bloq = bloq_m.get(e)->bloq;
      ofVec2f _bloc( bloq->loc );
      _bloc.x *= render_data->width;
      _bloc.y *= render_data->height;

      b2ParticleSystem* b2ps = system<ParticleSystem>()->b2_particles();
      FisicaSystem* fisica = system<FisicaSystem>();

      int32 n = b2ps->GetParticleCount();
      b2Vec2 *locs = b2ps->GetPositionBuffer(); 
      ofVec2f _ploc;
      for (int i = 0; i < n; i++)
      {
        fisica->world2screen( locs[i], _ploc );
        float d = abs( _bloc.distance( _ploc ) );
        if ( d < pvs_data->radius * render_data->height )
          pvs_data->volume++;
      }

      pvs_data->volume_i += (pvs_data->volume - pvs_data->volume_prev) * 0.2;
      pvs_data->volume_n = pvs_data->volume /n;
      pvs_data->volume_n_i = pvs_data->volume_i /n;

      TS_STOP("ParticleVolumeSensorSystem"); 
    }; 

    virtual void renderEntity(Entity &e)
    {
      ParticleVolumeSensorComponent* pvs_data = particle_volume_sensor_m.get(e);
      if ( !pvs_data->render ) return;

      RenderComponent* render_data = component<RenderComponent>("output");

      Bloq* bloq = bloq_m.get(e)->bloq;
      ofVec2f loc( bloq->loc_i ); 
      loc.x *= render_data->width;
      loc.y *= render_data->height;

      float r = pvs_data->radius * render_data->height;

      ofPushStyle();
      ofSetColor( ofColor::orange );
      ofNoFill();
      ofCircle( loc, r );
      ofFill();
      ofCircle( loc, r * pvs_data->volume_n_i * pvs_data->vol_view_mult );
      ofPopStyle();
    };

  private:

    ComponentMapper<ParticleVolumeSensorComponent> particle_volume_sensor_m;
    ComponentMapper<BloqComponent> bloq_m;

};
