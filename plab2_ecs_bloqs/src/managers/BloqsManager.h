#pragma once

#include <Artemis/Artemis.h>
#include "ECS.h"
#include "bloqs/Bloq.h"
#include "managers/ComponentFactory.h"

class BloqsManager
{
  public:

    BloqsManager(){};
    ~BloqsManager()
    {
      ecs = NULL;
    };

    void init( ECS* ecs )
    {
      this->ecs = ecs;
      config = load("components.cfg");
    };

    artemis::Entity* make_entity( Bloq& bloq )
    {
      int bloq_id = bloq.id;

      if ( has_entity( bloq_id ) )
      {
        artemis::Entity& e = ecs->entities()->getEntity( entities_by_bloq_id[ bloq_id ] );
        ofLogNotice("BloqsManager") << "make_entity by bloq id " << bloq_id << ": entity " << e.getId() << " already exists"; 
        return &e;
      }

      vector<string> components_cfg = config[ bloq_id ];

      if ( ! components_cfg.size() )
      {
        ofLogNotice("BloqsManager") << "make_entity by bloq id " << bloq_id << ": components config not found"; 
        return NULL;
      }

      vector<artemis::Component*> components;
      for ( int i = 0; i < components_cfg.size(); i++ )
      {
        string comp_name = components_cfg[i];
        artemis::Component* comp = component_factory.make( comp_name );
        if ( comp == NULL ) 
          continue;
        components.push_back( comp );
        if ( comp_name == "bloq" )
          ((BloqComponent*)comp)->update( &bloq );
      }

      if ( ! components.size() )
      {
        ofLogNotice("BloqsManager") << "make_entity by bloq id " << bloq_id << ": components names not found"; 
        return NULL;
      }

      artemis::Entity& e = ecs->entities()->create();
      ofLogNotice("BloqsManager") << "make_entity by bloq id " << bloq_id << ": entity " << e.getId(); 

      for ( int i = 0; i < components.size(); i++ )
        e.addComponent( components[i] );
      e.refresh();

      entities_by_bloq_id[ bloq_id ] = e.getId();
      return &e;
    }; 

    void update_entity( Bloq& bloq )
    {
      int bloq_id = bloq.id;

      if ( ! has_entity( bloq_id ) )
      {
        ofLogNotice("BloqsManager") << "update_entity bloq id " << bloq_id << ": entity not found"; 
        return;
      }

      artemis::Entity& e = ecs->entities()->getEntity( entities_by_bloq_id[ bloq_id ] );
      ofLogNotice("BloqsManager") << "update_entity bloq id " << bloq_id << ": entity " << e.getId(); 

      BloqComponent* bloq_comp = (BloqComponent*)e.getComponent<BloqComponent>();

      bloq_comp->update( &bloq );
    };

    void remove_entity( int bloq_id )
    {

      if ( ! has_entity( bloq_id ) )
      {
        ofLogNotice("BloqsManager") << "remove_entity by bloq id " << bloq_id << ": entity not found"; 
        return;
      }

      artemis::Entity& e = ecs->entities()->getEntity( entities_by_bloq_id[ bloq_id ] );
      ofLogNotice("BloqsManager") << "remove_entity by bloq id " << bloq_id << ": entity " << e.getId(); 
      ecs->entities()->remove( e );

      entities_by_bloq_id.erase(bloq_id);
    };

  private:

    ECS* ecs;

    ComponentFactory component_factory;

    //component names by bloq id
    map< int,vector<string> > config;

    //entity id by bloq id
    map< int,int > entities_by_bloq_id;

    bool has_entity( int bloq_id )
    {
      return entities_by_bloq_id.find( bloq_id ) != entities_by_bloq_id.end(); 
    };

    map< int,vector<string> > load( string filename )
    {
      ofLogNotice("BloqsManager") << "load " << filename; 

      map< int,vector<string> > config;

      ofFile file(filename);
      if ( !file.exists() )
      {
        ofLogNotice("BloqsManager") << "\t file is missing";
        return;
      }

      ofBuffer buffer(file);
      for ( ofBuffer::Line it = buffer.getLines().begin(); it != buffer.getLines().end(); ++it )
      {
        string line = it.asString();
        if ( line.empty() ) continue;

        ofLogNotice("BloqsManager") << "\t line " << line; 
        vector<string> cfg = ofSplitString( line, "=" );
        if ( cfg.size() != 2 ) 
        {
          ofLogNotice("BloqsManager") << "\t config line " << line << " lenght != 2 "; 
          continue;
        }
        int bloq_id = ofToInt( cfg[0] );
        vector<string> components = ofSplitString( cfg[1], "," );
        ofLogNotice("BloqsManager") << "\t bloq id " << bloq_id << " components: " << ofToString(components); 

        config[ bloq_id ] = components;
      }
      return config;
    };
};

 
