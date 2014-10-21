#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECS.h"
#include "managers/ComponentFactory.h"

class SimManager
{
  public:

    SimManager(){};
    ~SimManager()
    {
      ecs = NULL;
    };

    void init( ECS* ecs, ofxJSONElement* config )
    {
      this->ecs = ecs;
      comp_ids = parse_config( config );
    };

    artemis::Entity* make_entity()
    {
      if ( ! comp_ids.size() )
      {
        ofLogNotice("SimManager") << "make_entity components config not found"; 
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
        ofLogNotice("SimManager") << "make_entity components ids not found"; 
        return NULL;
      }

      artemis::Entity& e = ecs->entities()->create();
      ofLogNotice("SimManager") << "make_entity " << e.getId(); 

      for ( int i = 0; i < components.size(); i++ )
        e.addComponent( components[i] );
      e.refresh();

      return &e;
    }; 

  private:

    ECS* ecs;

    ComponentFactory component_factory;

    //list of sim components
    vector<string> comp_ids;

    vector<string> parse_config( ofxJSONElement* config )
    {
      vector<string> comp_ids;

      ofxJSONElement& cfg = *config;
      const Json::Value& _comp_ids = cfg["sim"];
      for ( int i = 0; i < _comp_ids.size(); ++i )
        comp_ids.push_back( _comp_ids[i].asString() );

      return comp_ids;
    };

};

 
