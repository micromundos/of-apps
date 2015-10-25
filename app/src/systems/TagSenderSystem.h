#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "bloqs/Bloq.h"
#include "events/BloqEvents.h"

using namespace artemis;

class TagSenderSystem : public ECSsystem 
{ 

  public:

    TagSenderSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<TagSenderComponent>();
    };

    virtual void initialize() 
    {
      tag_sender_m.init( *world );
    };

    virtual void added(Entity &e)
    {
      osc_sender.setup( tag_sender_m.get(e)->host, tag_sender_m.get(e)->port );
      ofAddListener( BloqEvents::added, this,   &TagSenderSystem::bloq_added );
      ofAddListener( BloqEvents::updated, this, &TagSenderSystem::bloq_updated );
      ofAddListener( BloqEvents::removed, this, &TagSenderSystem::bloq_removed );
    };

    virtual void removed(Entity &e)
    {
      ofRemoveListener( BloqEvents::added, this,   &TagSenderSystem::bloq_added );
      ofRemoveListener( BloqEvents::updated, this, &TagSenderSystem::bloq_updated );
      ofRemoveListener( BloqEvents::removed, this, &TagSenderSystem::bloq_removed );
    };

    virtual void processEntity(Entity &e) 
    {}; 

    virtual void renderEntity(Entity &e)
    {};

  private:

    ComponentMapper<TagSenderComponent> tag_sender_m;
  
    ofxOscSender  osc_sender;
 
    void bloq_added( Bloq& bloq )
    {
      send_osc( bloq, "/add" );
    };
    
    void bloq_updated( Bloq& bloq )
    {
      send_osc( bloq, "/update" );
    };
    
    void bloq_removed( string& bloq_id )
    {
      send_osc_remove( bloq_id );
    };
  
    //  osc
  
    void send_osc( Bloq& bloq, string _address )
    {
      ofxOscMessage m;
      m.setAddress( _address );

      m.addStringArg( bloq.id );
      m.addFloatArg( bloq.loc.x );
      m.addFloatArg( bloq.loc.y );
      m.addFloatArg( bloq.radians );

      //TODO interpolated
      //m.addFloatArg( bloq.loc_i.x );
      //m.addFloatArg( bloq.loc_i.y );
      //m.addFloatArg( bloq.radians_i );

      osc_sender.sendMessage(m);
    };
    
    void send_osc_remove( string bloq_id )
    {
      ofxOscMessage m;
      m.setAddress("/remove");
      m.addStringArg( bloq_id );
      osc_sender.sendMessage(m);
    };

};

