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
      server.setName( syphon_data->src );
      client.setup();
      client.set( syphon_data->dst, "Unity" );
    }; 

    virtual void processEntity(Entity &e) 
    {
      SyphonComponent* syphon_data = syphon_m.get(e);
      if ( ! syphon_data->send ) return;

      DepthComponent* depth_data = component<DepthComponent>("input");
      if ( ! depth_data->dirty ) return;

      TS_START("SyphonSystem send"); 
      DepthProcessingComponent* depth_proc_data = component<DepthProcessingComponent>("input");
      server.publishTexture( &depth_proc_data->surfaces().get_debug().get() );
      TS_STOP("SyphonSystem send");
    }; 

    virtual void renderEntity(Entity &e)
    {
      SyphonComponent* syphon_data = syphon_m.get(e);
      if ( ! syphon_data->receive ) return;

      TS_START("SyphonSystem receive"); 
      RenderComponent* render_data = component<RenderComponent>("output");
      client.draw( 0, 0, render_data->width, render_data->height );
      TS_STOP("SyphonSystem receive");
    };

  private:

    bool inited;
    ofxSyphonServer server;
    ofxSyphonClient client;

    ComponentMapper<SyphonComponent> syphon_m;

};

