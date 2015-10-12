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
  

    //{ bloq_id : entity_id }
    map< string,int > bloqs_by_id;


    void init( ConfigComponent* config_data )
    {
      bloq_maker.init( world, ((ComponentFactory*)component_factory), config_data->config.game()["bloqs"], config_data->config.settings()["params"]["app"]["port"].asInt() );

      ofAddListener( BloqEvents::added, this, &BloqMakerSystem::bloq_added );
      ofAddListener( BloqEvents::updated, this, &BloqMakerSystem::bloq_updated );
      ofAddListener( BloqEvents::removed, this, &BloqMakerSystem::bloq_removed );

    };


    void bloq_added( Bloq& bloq )
    {
      //ofLogNotice("BloqMakerSystem") 
        //<< "bloq_added: " 
        //<< bloq.id;

      make_entity( bloq );
    };

    void bloq_updated( Bloq& bloq )
    {
      update_entity( bloq );
    };

    void bloq_removed( string& bloq_id )
    {
      //ofLogNotice("BloqMakerSystem") 
        //<< "bloq_removed: " 
        //<< bloq_id;
      remove_entity( bloq_id );
    };

    void make_entity( Bloq& bloq )
    {
      string bloq_id = bloq.id;

      if ( bloq_maker.has_entity( bloq_id ) )
      {
        ofLogWarning("BloqMakerSystem") << "make_entity by id/tag " << bloq_id << ": entity already exists";
        return;
      }

      artemis::Entity* e = bloq_maker.make_entity( bloq_id );

      if ( e == NULL ) return;

      //add default bloq component
      ECScomponent* bloq_comp = component_factory->make( "bloq" );
      ((BloqComponent*)bloq_comp)->update( &bloq );
      e->addComponent( bloq_comp );
      e->refresh();

      bloqs_by_id[ bloq_id ] = e->getId();
    };  

    void remove_entity( string bloq_id )
    {
      bloq_maker.remove_entity( bloq_id );

      if ( ! has_entity( bloq_id ) )
      {
        ofLogWarning("BloqMakerSystem") << "remove_entity by bloq id " << bloq_id << ": entity not found"; 
        return;
      }

      bloqs_by_id.erase(bloq_id);
    };

    void update_entity( Bloq& bloq )
    {
      string bloq_id = bloq.id;

      if ( ! has_entity( bloq_id ) )
        return;

      artemis::Entity& e = entities()->getEntity( bloqs_by_id[ bloq_id ] );

      BloqComponent* bloq_comp = (BloqComponent*)e.getComponent<BloqComponent>();

      bloq_comp->update( &bloq );
    };

    bool has_entity( string bloq_id )
    {
      return bloqs_by_id.find( bloq_id ) != bloqs_by_id.end(); 
    };

};

