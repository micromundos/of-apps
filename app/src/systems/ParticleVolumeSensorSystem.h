#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "PDraw.h"

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
      
      color_area = ofColor(236,255,137,200);
      
    };

    virtual void removed(Entity &e) 
    {};

    virtual void added(Entity &e) 
    {
      ParticleVolumeSensorComponent* pvs_data = particle_volume_sensor_m.get(e);
      pvs_data->volume = 0.0;
      pvs_data->volume_n = 0.0;
      pvs_data->volume_i = 0.0;
    }; 

    virtual void processEntity(Entity &e) 
    {
      TS_START("ParticleVolumeSensorSystem"); 

      RenderComponent* render_data = component<RenderComponent>("output");

      ParticleVolumeSensorComponent* pvs_data = particle_volume_sensor_m.get(e);

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

      pvs_data->volume_i += (pvs_data->volume - pvs_data->volume_i) * 0.1;
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
      /*
      ofColor col( ofColor::orange );
      ofPushStyle();
      ofSetColor( col );
      ofNoFill();
      ofCircle( loc, r );
      ofFill();
      ofEnableAlphaBlending();
      col.a = 200;
      ofSetColor( col );
      ofCircle( loc, r * pvs_data->volume_n_i );
      ofDisableAlphaBlending();
      ofPopStyle();*/
      
      ofPushMatrix();
      ofTranslate(loc.x,loc.y);
      ofSetColor(color_area);
      ofFill();
      ofCircle( 0.0,0.0, r);
      float _radius = r * pvs_data->volume_n_i;
      float _vol_radius = r+20;
      
      draw_volume.circle(_vol_radius,_vol_radius+50,40,ofColor(0,180),ofColor(0,180),0,360,true);
      float _vol = pvs_data->volume_n_i*360.0;

      draw_volume.circle(_vol_radius,_vol_radius+10,40,ofColor(255,0,0,180),ofColor(255,0,180),0,_vol,true);

      
      /*
      draw_area.circle(emitter_data->draw_radius,emitter_data->draw_radius+emitter_data->draw_weight,draw_resolution,direction_color_l,direction_color_l,10,20,true);*/
      

      ofPopMatrix();
      
      
      
    };

  private:

    ComponentMapper<ParticleVolumeSensorComponent> particle_volume_sensor_m;
    ComponentMapper<BloqComponent> bloq_m;
  
    PDraw  draw_volume;
    ofColor color_area;
  



};

