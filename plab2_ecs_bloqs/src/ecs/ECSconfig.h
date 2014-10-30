#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECS.h"
#include "components/ComponentFactory.h"
#include "bloqs/Bloq.h"

class ECSconfig
{
  public:

    ECSconfig(){};
    ~ECSconfig()
    {
      ecs = NULL;
    };

    void make( ECS* ecs, ofxJSONElement* config )
    {
      this->ecs = ecs;
      entities_cfg = parse_config(config);

      for ( map< string,vector<string> >::iterator it = entities_cfg.begin(); it != entities_cfg.end(); ++it )
        make_entity( it->first );
    };

    artemis::Entity* make_entity( string e_id )
    {
      ofLogNotice("ECSconfig") << "make_entity by id/tag " << e_id; 

      //TODO make muchas entities del mismo tipo, pero el e_id es tag... ver unity prefabs
      if ( has_entity( e_id ) )
      {
        artemis::Entity& e = ecs->entities()->getEntity( entities_ids[ e_id ] );
        ofLogNotice("ECSconfig") << "make_entity by id/tag " << e_id << ": entity " << e.getId() << " already exists"; 
        return &e;
      }

      vector<string> comp_ids = entities_cfg[ e_id ];

      if ( ! comp_ids.size() )
      {
        ofLogNotice("ECSconfig") << "make_entity by id/tag " << e_id << ": components config not found"; 
        return NULL;
      }

      vector<artemis::Component*> components;

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
        ofLogNotice("ECSconfig") << "make_entity by id/tag " << e_id << ": components ids not found"; 
        return NULL;
      }

      artemis::Entity& e = ecs->entities()->create();
      ofLogNotice("ECSconfig") << "\tentity " << e_id << " made: entity " << e.getId(); 

      for ( int i = 0; i < components.size(); i++ )
        e.addComponent( components[i] );
      e.refresh();

      ecs->tags()->subscribe( e_id, e );

      entities_ids[ e_id ] = e.getId();

      return &e;
    }; 

    void remove_entity( string e_id )
    {
      if ( ! has_entity( e_id ) )
      {
        ofLogNotice("ECSconfig") << "remove_entity by id/tag " << e_id << ": entity not found"; 
        return;
      }

      artemis::Entity& e = ecs->entities()->getEntity( entities_ids[ e_id ] );
      ofLogNotice("ECSconfig") << "remove_entity by id/tag " << e_id << ": entity " << e.getId(); 
      ecs->entities()->remove( e );

      entities_ids.erase( e_id );
    };

  private:

    ECS* ecs;
    ComponentFactory component_factory;

    //{ component_id: [entities_ids] }
    map< string,vector<string> > entities_cfg;

    //{ tag:id }
    map< string,int > entities_ids;

    bool has_entity( string e_id )
    {
      return entities_ids.find( e_id ) != entities_ids.end(); 
    };

    map< string,vector<string> > parse_config( ofxJSONElement* config )
    {
      map< string,vector<string> > entities_cfg;

      ofxJSONElement& cfg = *config;
      const Json::Value& _entities = cfg["entities"];
      for ( int i = 0; i < _entities.size(); ++i )
      {

        string e_id = _entities[i]["id"].asString();
        vector<string> comp_ids;
        const Json::Value& _comp_ids = _entities[i]["components"];
        for ( int j = 0; j < _comp_ids.size(); ++j )
        {
          comp_ids.push_back( _comp_ids[j].asString() );
        }

        entities_cfg[ e_id ] = comp_ids;
      }

      return entities_cfg;
    };

};

 
