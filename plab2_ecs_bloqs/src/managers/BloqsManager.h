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

    void init( ECS* ecs, ofxJSONElement* config )
    {
      this->ecs = ecs;
      components_cfg = parse_config( config );
    };

    artemis::Entity* make_entity( Bloq& bloq )
    {
      string bloq_id = bloq.id;

      if ( has_entity( bloq_id ) )
      {
        artemis::Entity& e = ecs->entities()->getEntity( entities_by_bloq_id[ bloq_id ] );
        ofLogNotice("BloqsManager") << "make_entity by bloq id " << bloq_id << ": entity " << e.getId() << " already exists"; 
        return &e;
      }

      vector<string> comp_ids = components_cfg[ bloq_id ];

      if ( ! comp_ids.size() )
      {
        ofLogNotice("BloqsManager") << "make_entity by bloq id " << bloq_id << ": components config not found"; 
        return NULL;
      }

      vector<artemis::Component*> components;
      for ( int i = 0; i < comp_ids.size(); i++ )
      {
        string comp_id = comp_ids[i];
        artemis::Component* comp = component_factory.make( comp_id );
        if ( comp == NULL ) 
          continue;
        components.push_back( comp );
        if ( comp_id == "bloq" )
          ((BloqComponent*)comp)->update( &bloq );
      }

      if ( ! components.size() )
      {
        ofLogNotice("BloqsManager") << "make_entity by bloq id " << bloq_id << ": components ids not found"; 
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
      string bloq_id = bloq.id;

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

    void remove_entity( string bloq_id )
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

    //component ids by bloq id
    map< string,vector<string> > components_cfg;

    //entity id by bloq id
    map< string,int > entities_by_bloq_id;

    bool has_entity( string bloq_id )
    {
      return entities_by_bloq_id.find( bloq_id ) != entities_by_bloq_id.end(); 
    };

    map< string,vector<string> > parse_config( ofxJSONElement* config )
    {
      map< string,vector<string> > components_cfg;

      ofxJSONElement& cfg = *config;
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

    //map< int,vector<string> > load( string filename )
    //{
      //ofLogNotice("BloqsManager") << "load " << filename; 

      //map< int,vector<string> > components_cfg;

      //ofFile file(filename);
      //if ( !file.exists() )
      //{
        //ofLogNotice("BloqsManager") << "\t file is missing";
        //return;
      //}

      //ofBuffer buffer(file);
      //for ( ofBuffer::Line it = buffer.getLines().begin(); it != buffer.getLines().end(); ++it )
      //{
        //string line = it.asString();
        //if ( line.empty() || line[0] == '#' ) 
          //continue;

        //ofLogNotice("BloqsManager") << "\t line " << line; 
        //vector<string> cfg = ofSplitString( line, "=" );
        //if ( cfg.size() != 2 ) 
        //{
          //ofLogNotice("BloqsManager") << "\t components_cfg line " << line << " lenght != 2 "; 
          //continue;
        //}
        //int bloq_id = ofToInt( cfg[0] );
        //vector<string> comp_ids = ofSplitString( cfg[1], "," );
        //ofLogNotice("BloqsManager") << "\t bloq id " << bloq_id << " components: " << ofToString(comp_ids); 

        //components_cfg[ bloq_id ] = comp_ids;
      //}
      //return components_cfg;
    //};
};

 
