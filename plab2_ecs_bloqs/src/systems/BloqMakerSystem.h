#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/Components.h"
#include "components/ComponentFactory.h"

using namespace artemis;

class BloqMakerSystem : public ECSsystem 
{ 

  public:

    BloqMakerSystem() 
    {
      addComponentType<BloqMakerComponent>();
      addComponentType<BloqEventsComponent>();
      addComponentType<ConfigComponent>();
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

  private:

    ComponentMapper<BloqMakerComponent> bloq_maker_m;
    ComponentMapper<BloqEventsComponent> bloq_events_m;
    ComponentMapper<ConfigComponent> config_m;


    ComponentFactory component_factory;
    //component ids by bloq id
    map< string,vector<string> > components_cfg;
    //entity id by bloq id
    map< string,int > entities_by_bloq_id;


    void init( ConfigComponent* config_data, BloqEventsComponent* bloq_events )
    {
      components_cfg = parse_config( config_data->config );

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


    artemis::Entity* make_entity( Bloq& bloq )
    {
      string bloq_id = bloq.id;

      if ( has_entity( bloq_id ) )
      {
        artemis::Entity& e = entities()->getEntity( entities_by_bloq_id[ bloq_id ] );
        ofLogNotice("BloqMakerSystem") << "make_entity by bloq id " << bloq_id << ": entity " << e.getId() << " already exists"; 
        return &e;
      }

      vector<string> comp_ids = components_cfg[ bloq_id ];

      if ( ! comp_ids.size() )
      {
        ofLogNotice("BloqMakerSystem") << "make_entity by bloq id " << bloq_id << ": components config not found"; 
        return NULL;
      }

      vector<artemis::Component*> components;

      //add default components
      artemis::Component* bloq_comp = component_factory.make( "bloq" );
      components.push_back( bloq_comp );
      ((BloqComponent*)bloq_comp)->update( &bloq );

      //add config components
      for ( int i = 0; i < comp_ids.size(); i++ )
      {
        string comp_id = comp_ids[i];
        artemis::Component* comp = component_factory.make( comp_id );
        if ( comp != NULL ) 
          components.push_back( comp );
      }

      if ( ! components.size() )
      {
        ofLogNotice("BloqMakerSystem") << "make_entity by bloq id " << bloq_id << ": components ids not found"; 
        return NULL;
      }

      artemis::Entity& e = entities()->create();
      ofLogNotice("BloqMakerSystem") << "make_entity by bloq id " << bloq_id << ": entity " << e.getId(); 

      for ( int i = 0; i < components.size(); i++ )
        e.addComponent( components[i] );
      e.refresh();

      entities_by_bloq_id[ bloq_id ] = e.getId();
      return &e;
    }; 

    void update_entity( Bloq& bloq )
    {
      string bloq_id = bloq.id;

      if ( ! has_entity( bloq_id ) )
      {
        //ofLogNotice("BloqMakerSystem") << "update_entity bloq id " << bloq_id << ": entity not found"; 
        return;
      }

      artemis::Entity& e = entities()->getEntity( entities_by_bloq_id[ bloq_id ] );
      //ofLogNotice("BloqMakerSystem") << "update_entity bloq id " << bloq_id << ": entity " << e.getId(); 

      BloqComponent* bloq_comp = (BloqComponent*)e.getComponent<BloqComponent>();

      bloq_comp->update( &bloq );
    };

    void remove_entity( string bloq_id )
    {

      if ( ! has_entity( bloq_id ) )
      {
        ofLogNotice("BloqMakerSystem") << "remove_entity by bloq id " << bloq_id << ": entity not found"; 
        return;
      }

      artemis::Entity& e = entities()->getEntity( entities_by_bloq_id[ bloq_id ] );
      ofLogNotice("BloqMakerSystem") << "remove_entity by bloq id " << bloq_id << ": entity " << e.getId(); 
      entities()->remove( e );

      entities_by_bloq_id.erase(bloq_id);
    };

    bool has_entity( string bloq_id )
    {
      return entities_by_bloq_id.find( bloq_id ) != entities_by_bloq_id.end(); 
    };

    map< string,vector<string> > parse_config( ofxJSONElement& cfg )
    {
      map< string,vector<string> > components_cfg;

      const Json::Value& _bloqs = cfg["bloqs"];
      for ( int i = 0; i < _bloqs.size(); ++i )
      {

        string bloq_id = _bloqs[i]["id"].asString();
        vector<string> comp_ids;
        const Json::Value& _comp_ids = _bloqs[i]["components"];
        for ( int j = 0; j < _comp_ids.size(); ++j )
        {
          comp_ids.push_back( _comp_ids[j].asString() );
        }

        components_cfg[ bloq_id ] = comp_ids;
      }

      return components_cfg;
    };

};

