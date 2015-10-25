#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "PlabComponentFactory.h"
#include "bloqs/Bloq.h"
#include "events/BloqEvents.h"

using namespace artemis;

class BloqMakerSystem : public ECSsystem 
{ 

  public:

    BloqMakerSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<BloqMakerComponent>();
      addComponentType<ConfigComponent>();

      component_factory = new PlabComponentFactory();
    };

    virtual ~BloqMakerSystem()
    {
      delete component_factory;
    };

    virtual void initialize() 
    {
      bloq_maker_m.init( *world );
      config_m.init( *world );
    };

    virtual void added(Entity &e) 
    {
      init( config_m.get(e) );
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      artemis::EntityProcessingSystem::processEntities( bag );
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("BloqMakerSystem") << "process entity " << e.getId();
      //bloq_maker_m.get(e)->data;
    }; 

    virtual void renderEntity(Entity &e)
    {};

  private:

    ComponentMapper<BloqMakerComponent> bloq_maker_m;
    ComponentMapper<ConfigComponent> config_m;

    ECmaker bloq_maker;
    PlabComponentFactory* component_factory;
  
    void init( ConfigComponent* config_data )
    {
      bloq_maker.init( world, 
          ((ComponentFactory*)component_factory), 
          config_data->config.game()["bloqs"], 
          config_data->config.settings()["params"]["app"]["port"].asInt() );

      ofAddListener( BloqEvents::added, this, &BloqMakerSystem::make_bloq_entity );
      ofAddListener( BloqEvents::updated, this, &BloqMakerSystem::update_bloq_entity );
      ofAddListener( BloqEvents::removed, this, &BloqMakerSystem::remove_bloq_entity );
    };

    void make_bloq_entity( Bloq& bloq )
    {
      string bloq_id = bloq.id;

      if ( bloq_maker.has_entity( bloq_id ) )
      {
        ofLogWarning("BloqMakerSystem") << "make_bloq_entity by id/tag " << bloq_id << ": entity already exists";
        return;
      }

      artemis::Entity* e = bloq_maker.make_entity( bloq_id );

      if ( e == NULL ) return;

      //add default bloq component
      ECScomponent* bloq_comp = component_factory->make( "bloq" );
      ((BloqComponent*)bloq_comp)->update( bloq );
      e->addComponent( bloq_comp );
      e->refresh();
    };  

    void remove_bloq_entity( string& bloq_id )
    {
      if ( !bloq_maker.has_entity( bloq_id ) )
        return;
      bloq_maker.remove_entity( bloq_id );
    };

    void update_bloq_entity( Bloq& bloq )
    {
      string bloq_id = bloq.id;

      artemis::Entity* e = bloq_maker.get_entity( bloq_id );

      if ( e == NULL )
      {
        //ofLogWarning("BloqMakerSystem") << "update_bloq_entity by bloq id " << bloq_id << ": entity not found"; 
        return;
      }

      BloqComponent* bloq_comp = (BloqComponent*)(e->getComponent<BloqComponent>());

      bloq_comp->update( bloq );
    };

};

