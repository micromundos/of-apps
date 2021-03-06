#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

using namespace artemis;

class TemplateSystem : public ECSsystem 
{ 

  public:

    TemplateSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<TemplateComponent>();
    };

    virtual void initialize() 
    {
      template_m.init( *world );
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
    {}; 

    virtual void processEntity(Entity &e) 
    {
      //template_m.get(e)->data;
    }; 

    virtual void renderEntity(Entity &e)
    {};

  private:

    ComponentMapper<TemplateComponent> template_m;

};

