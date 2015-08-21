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
    {
    };

    virtual void added(Entity &e) 
    {
    }; 

    virtual void processEntity(Entity &e) 
    {
      DepthProcessingComponent* depth_proc_data = require_component<DepthProcessingComponent>("input");
      syphon_server.publishTexture(&depth_proc_data->height_map().get());
      //  DepthComponent* depth_data = depth_m.get(e);
     // ofLogNotice("SyphonSystem") << "process entity " << e.getId();
      //syphon_m.get(e)->data;
    }; 

    virtual void renderEntity(Entity &e)
    {};

  private:

    ComponentMapper<SyphonComponent> syphon_m;
    ComponentMapper<DepthProcessingComponent> depth_processing_m;
    ofxSyphonServer                  syphon_server;

};

