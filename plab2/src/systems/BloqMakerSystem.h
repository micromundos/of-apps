#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "ecs/Components.h"
#include "ecs/PlabComponentFactory.h"

using namespace artemis;

class BloqMakerSystem : public ECSsystem 
{ 

  public:

    BloqMakerSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<BloqMakerComponent>();
      addComponentType<BloqEventsComponent>();
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
      bloq_events_m.init( *world );
      config_m.init( *world );
    };

    virtual void added(Entity &e) 
    {
      init( config_m.get(e), bloq_events_m.get(e) );
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("BloqMakerSystem") << "process entity " << e.getId();
      //bloq_maker_m.get(e)->data;
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      for (int i=0;i<bag.getCount();i++)
        processEntity( *bag.get(i) );
    };

    virtual void render_entity(Entity &e)
    {};

  private:

    ComponentMapper<BloqMakerComponent> bloq_maker_m;
    ComponentMapper<BloqEventsComponent> bloq_events_m;
    ComponentMapper<ConfigComponent> config_m;

    ECmaker bloq_maker;
    PlabComponentFactory* component_factory;

    //{ bloq_id : entity_id }
    map< string,int > bloqs_by_id;


    void init( ConfigComponent* config_data, BloqEventsComponent* bloq_events )
    {
      bloq_maker.init( world, ((ComponentFactory*)component_factory), config_data->config.json()["bloqs"] );

      ofAddListener( bloq_events->added, this, &BloqMakerSystem::bloq_added );
      ofAddListener( bloq_events->updated, this, &BloqMakerSystem::bloq_updated );
      ofAddListener( bloq_events->removed, this, &BloqMakerSystem::bloq_removed );

    };


    void bloq_added( Bloq& bloq )
    {
      make_entity( bloq );
    };

    void bloq_updated( Bloq& bloq )
    {
      update_entity( bloq );
    };

    void bloq_removed( string& bloq_id )
    {
      remove_entity( bloq_id );
    };


    void make_entity( Bloq& bloq )
    {
      string bloq_id = bloq.id;

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
        ofLogNotice("BloqMakerSystem") << "remove_entity by bloq id " << bloq_id << ": entity not found"; 
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

