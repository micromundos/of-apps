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
      syphon_server.setName("Plab");
    };

    virtual void removed(Entity &e) 
    {
    };

    virtual void added(Entity &e) 
    {
    }; 

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth_data = require_component<DepthComponent>("input");
      if ( ! depth_data->dirty ) return;
      TS_START("SyphonSystem"); 
      DepthProcessingComponent* depth_proc_data = require_component<DepthProcessingComponent>("input");
      syphon_server.publishTexture(&depth_proc_data->surfaces().get_debug().get());
      TS_STOP("SyphonSystem");
    }; 

    virtual void renderEntity(Entity &e)
    {};

  private:

    ComponentMapper<SyphonComponent> syphon_m;
    ComponentMapper<DepthProcessingComponent> depth_processing_m;
    ofxSyphonServer                  syphon_server;

};

