#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "components/Components.h"

using namespace artemis;

class KeyboardSystem : public ECSsystem 
{ 

  public:

    KeyboardSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<KeyboardComponent>();
    };

    virtual void initialize() 
    {
      keyboard_m.init( *world );
    };

    virtual void added(Entity &e) 
    {
      //keyboard_m.get(e)->data;
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("KeyboardSystem") << "process entity " << e.getId();
      //keyboard_m.get(e)->data;
    };

    virtual void render_entity(Entity &e)
    {};

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      int len = bag.getCount();
      for ( int i = 0; i < len; i++ )
        processEntity( *bag.get(i) );
    };

    virtual void render_entities( ImmutableBag<Entity*>& bag ) 
    {
      int len = bag.getCount();
      for ( int i = 0; i < len; i++ )
        render_entity( *bag.get(i) );
    }; 

  private:

    ComponentMapper<KeyboardComponent> keyboard_m;

};

