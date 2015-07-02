#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

using namespace artemis;

class TagsReceiverSystem : public ECSsystem 
{ 

  public:

    TagsReceiverSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<TagsReceiverComponent>();
    };

    virtual void initialize() 
    {
      tags_receiver_m.init( *world );
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
      //tags_receiver_m.get(e)->data;
    }; 

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("TagsReceiverSystem") << "process entity " << e.getId();
      //tags_receiver_m.get(e)->data;
    }; 

    virtual void renderEntity(Entity &e)
    {};

  private:

    ComponentMapper<TagsReceiverComponent> tags_receiver_m;

};

