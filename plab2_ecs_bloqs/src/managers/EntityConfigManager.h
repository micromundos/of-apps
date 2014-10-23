#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECS.h"
#include "components/ComponentFactory.h"

class EntityConfigManager
{
  public:

    EntityConfigManager(){};
    ~EntityConfigManager(){};

    void init( ECS* ecs, ofxJSONElement* config, string config_id )
    {
      if ( entity != NULL )
      {
        ofLogNotice("EntityConfigManager") << "init" << ", config id [" << config_id << "] entity already inited with id " << entity->getId(); 
        return;
      }

      comp_ids = parse_config( config, config_id );
      entity = make_entity( ecs, config_id );
    };

    template<typename TComponent>
    TComponent* component()
    {
      return entity != NULL ? (TComponent*)entity->getComponent<TComponent>() : NULL;
    };

  private:

    ComponentFactory component_factory;
    artemis::Entity* entity;
    vector<string> comp_ids;

    artemis::Entity* make_entity( ECS* ecs, string config_id )
    { 

      if ( ! comp_ids.size() )
      {
        ofLogNotice("EntityConfigManager") << "make_entity components config not found"; 
        return NULL;
      }

      vector<artemis::Component*> components;

      for ( int i = 0; i < comp_ids.size(); i++ )
      {
        string comp_id = comp_ids[i];
        artemis::Component* comp = component_factory.make( comp_id );
        if ( comp != NULL ) 
          components.push_back( comp );
      }

      if ( ! components.size() )
      {
        ofLogNotice("EntityConfigManager") << "make_entity components ids not found"; 
        return NULL;
      }

      artemis::Entity& e = ecs->entities()->create();
      ofLogNotice("EntityConfigManager") << "make_entity id " << e.getId(); 

      for ( int i = 0; i < components.size(); i++ )
        e.addComponent( components[i] );
      e.refresh();

      ecs->tags()->subscribe(config_id,e);

      return &e;
    };  

    vector<string> parse_config( ofxJSONElement* config, string config_id )
    {
      vector<string> comp_ids;

      ofxJSONElement& cfg = *config;
      const Json::Value& _comp_ids = cfg[ config_id ];
      for ( int i = 0; i < _comp_ids.size(); ++i )
        comp_ids.push_back( _comp_ids[i].asString() );

      return comp_ids;
    };

};

 
