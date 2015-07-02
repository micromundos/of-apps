#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

using namespace artemis;

class TagsProcessingSystem : public ECSsystem 
{ 

  public:

    TagsProcessingSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<TagsProcessingComponent>();
    };

    virtual void initialize() 
    {
      tags_processing_m.init( *world );
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

    virtual void added(Entity &e) 
    {
      //tags_processing_m.get(e)->data;
    }; 

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("TagsProcessingSystem") << "process entity " << e.getId();
      //tags_processing_m.get(e)->data;
    }; 

    virtual void renderEntity(Entity &e)
    {};

  private:

    ComponentMapper<TagsProcessingComponent> tags_processing_m;

};

