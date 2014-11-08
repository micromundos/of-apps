#pragma once

#include <Artemis/Artemis.h>
#include "components/ComponentFactory.h"

class ECSconfig
{
  public:

    ECSconfig(){};
    ~ECSconfig(){};

    void init( artemis::World* world, ofxJSONElement* config, string entities_cfg_name )
    {
      this->world = world;
      entities_cfg = parse_config( config, entities_cfg_name );
    }

    void make_all()
    {
      for ( map< string,Json::Value >::iterator it = entities_cfg.begin(); it != entities_cfg.end(); ++it )
        make_entity( it->first );
    };

    artemis::Entity* make_entity( string e_id )
    {
      ofLogNotice("ECSconfig") << "make_entity by id/tag " << e_id; 

      if ( has_entity( e_id ) )
      {
        artemis::Entity& e = world->getEntityManager()->getEntity( entities_ids[ e_id ] );
        ofLogNotice("ECSconfig") << "make_entity by id/tag " << e_id << ": entity " << e.getId() << " already exists"; 
        return &e;
      }

      Json::Value comps_cfg = entities_cfg[ e_id ];

      if ( ! comps_cfg.size() )
      {
        ofLogNotice("ECSconfig") << "make_entity by id/tag " << e_id << ": components config not found"; 
        return NULL;
      }

      vector<artemis::Component*> components;

      //add config components
      for ( int i = 0; i < comps_cfg.size(); i++ )
      {
        const Json::Value& comp_cfg = comps_cfg[i];
        ECScomponent* comp = component_factory.make( comp_cfg["id"].asString() );
        if ( comp != NULL ) 
        {
          comp->data( comp_cfg["data"] );
          components.push_back( comp );
        }
      }

      if ( ! components.size() )
      {
        ofLogNotice("ECSconfig") << "make_entity by id/tag " << e_id << ": components ids not found"; 
        return NULL;
      }

      artemis::Entity& e = world->getEntityManager()->create();
      ofLogNotice("ECSconfig") << "\tentity " << e_id << " made: entity " << e.getId(); 

      for ( int i = 0; i < components.size(); i++ )
        e.addComponent( components[i] );
      e.refresh();

      world->getTagManager()->subscribe( e_id, e );

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

      artemis::Entity& e = world->getEntityManager()->getEntity( entities_ids[ e_id ] );
      ofLogNotice("ECSconfig") << "remove_entity by id/tag " << e_id << ": entity " << e.getId(); 
      world->getEntityManager()->remove( e );

      entities_ids.erase( e_id );
    };

  private:

    artemis::World* world;
    ComponentFactory component_factory;
 
    //{ entity_id : [ components ] }
    map< string,Json::Value > entities_cfg;

    //{ tag : id }
    map< string,int > entities_ids;

    bool has_entity( string e_id )
    {
      return entities_ids.find( e_id ) != entities_ids.end(); 
    };

    map< string,Json::Value > parse_config( ofxJSONElement* config, string entities_cfg_name )
    {
      map< string,Json::Value > entities_cfg;

      ofxJSONElement& cfg = *config;
      const Json::Value& _entities = cfg[ entities_cfg_name ];

      for ( int i = 0; i < _entities.size(); ++i )
      {
        string e_id = _entities[i]["id"].asString();
        entities_cfg[ e_id ] = _entities[i]["components"];
      }

      return entities_cfg;
    };

};

 
