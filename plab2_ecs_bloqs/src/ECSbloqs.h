#pragma once

#include <Artemis/Artemis.h>
#include "ECS.h"
#include "bloqs/Bloq.h"
#include "components/ComponentFactory.h"

class ECSbloqs
{
  public:

    ECSbloqs(){};
    ~ECSbloqs()
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

      ofLogNotice("ECSbloqs") << "make_entity by bloq id " << ofToString(bloq_id); 

      if ( has_entity( bloq_id ) )
      {
        ofLogNotice("ECSbloqs") << "\t entity already exists"; 
        return &(ecs->entities()->getEntity( entities_by_bloq_id[ bloq_id ] ));
      }

      vector<string> components_cfg = config[ bloq_id ];

      if ( ! components_cfg.size() )
      {
        ofLogNotice("ECSbloqs") << "\t components config not found"; 
        return NULL;
      }

      vector<artemis::Component*> components;
      for ( int i = 0; i < components_cfg.size(); i++ )
      {
        artemis::Component* comp = component_factory.make( components_cfg[i], &bloq );
        if (comp != NULL) 
          components.push_back(comp);
      }

      if ( ! components.size() )
      {
        ofLogNotice("ECSbloqs") << "\t components ids not found"; 
        return NULL;
      }

      artemis::Entity& e = ecs->entities()->create();
      for ( int i = 0; i < components.size(); i++ )
        e.addComponent( components[i] );
      e.refresh();

      entities_by_bloq_id[ bloq_id ] = e.getId();
      return &e;
    }; 

    void update_entity( Bloq& bloq )
    {
      int bloq_id = bloq.id;

      ofLogNotice("ECSbloqs") << "update_entity bloq id " << ofToString(bloq_id); 

      if ( ! has_entity( bloq_id ) )
      {
        ofLogNotice("ECSbloqs") << "\t entity not found"; 
        return;
      }

      artemis::Entity& e = ecs->entities()->getEntity( entities_by_bloq_id[ bloq_id ] );
      BloqComponent* bloq_comp = (BloqComponent*)e.getComponent<BloqComponent>();

      bloq_comp->update( &bloq );
    };

    void remove_entity( int bloq_id )
    {
      ofLogNotice("ECSbloqs") << "remove_entity by bloq id " << ofToString(bloq_id); 

      if ( ! has_entity( bloq_id ) )
      {
        ofLogNotice("ECSbloqs") << "\t entity not found"; 
        return;
      }

      ecs->entities()->remove( ecs->entities()->getEntity( entities_by_bloq_id[ bloq_id ] ) );
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
      ofLogNotice("ECSbloqs") << "load " << filename; 

      map< int,vector<string> > config;

      ofFile file(filename);
      if ( !file.exists() )
      {
        ofLogNotice("ECSbloqs") << "\t file is missing";
        return;
      }

      ofBuffer buffer(file);
      for ( ofBuffer::Line it = buffer.getLines().begin(); it != buffer.getLines().end(); ++it )
      {
        string line = it.asString();
        if ( line.empty() ) continue;

        ofLogNotice("ECSbloqs") << "\t line " << line; 
        vector<string> cfg = ofSplitString( line, "=" );
        if ( cfg.size() != 2 ) 
        {
          ofLogNotice("ECSbloqs") << "\t config line " << line << " lenght != 2 "; 
          continue;
        }
        int bloq_id = ofToInt( cfg[0] );
        vector<string> components = ofSplitString( cfg[1], "," );
        ofLogNotice("ECSbloqs") << "\t bloq id " << ofToString(bloq_id) << " components: " << ofToString(components); 

        config[ bloq_id ] = components;
      }
      return config;
    };
};

 
