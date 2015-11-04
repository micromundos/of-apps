#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxSyphon.h"

using namespace artemis;

class SyphonSystem : public ECSsystem 
{ 

  public:

    SyphonSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<SyphonComponent>();
    };

    virtual void initialize() 
    {
      syphon_m.init( *world ); 
      inited = false;
    };

    virtual void removed(Entity &e) 
    {
      inited = false;
    };

    virtual void added(Entity &e) 
    {
      if (inited)
      {
        ofLogWarning("SyphonSystem") 
          << "singleton entity already added";
        return;
      }
      inited = true;

      SyphonComponent* syphon_data = syphon_m.get(e);

      syphon_data->server_surfaces.setName( syphon_data->sender_server_surfaces );
      syphon_data->server_depth_map.setName( syphon_data->sender_server_depth_map );

      syphon_data->client.setup();
      syphon_data->client.set( syphon_data->receiver_server, syphon_data->receiver_app );
    }; 

    virtual void processEntity(Entity &e) 
    {
      if ( !check_time(e) ) return;

      SyphonComponent* syphon_data = syphon_m.get(e);
      if ( ! syphon_data->send ) return;

      DepthComponent* depth_data = component<DepthComponent>("input");
      if ( ! depth_data->dirty ) return;

      TS_START("SyphonSystem send"); 

      DepthProcessingComponent* depth_proc_data = component<DepthProcessingComponent>("input");
      CamaraLucidaComponent* cml_data = component<CamaraLucidaComponent>("output");

      syphon_data->server_surfaces.publishTexture( &(depth_proc_data->surfaces().get_debug().get()) );

      syphon_data->server_depth_map.publishTexture( &(cml_data->cml->depth_camera()->get_float_tex_n()) );

      TS_STOP("SyphonSystem send");
    }; 

    virtual void renderEntity(Entity &e)
    {
      SyphonComponent* syphon_data = syphon_m.get(e);
      if ( ! syphon_data->receive ) return;

      TS_START("SyphonSystem receive"); 
      RenderComponent* render_data = component<RenderComponent>("output");
      syphon_data->client.draw( 0, 0, render_data->width, render_data->height );
      TS_STOP("SyphonSystem receive");
    };

  private:

    bool inited; 

    ComponentMapper<SyphonComponent> syphon_m;

    bool check_time(Entity &e) 
    {
      SyphonComponent* syphon_data = syphon_m.get(e);
      float micros_per_update = 1000000.0/syphon_data->fps;
      unsigned long long now = ofGetElapsedTimeMicros();
      unsigned long long dt = now - syphon_data->prev;

      if ( dt < micros_per_update )
        return false;

      syphon_data->prev = now;
      return true;
    }

};

