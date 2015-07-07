#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxOsc.h"
#include "bloqs/Tag.h"

using namespace artemis;

class TagsReceiverSystem : public ECSsystem 
{ 

  public:

    TagsReceiverSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<TagsReceiverComponent>();
    };

    virtual void initialize() 
    {
      tags_receiver_m.init( *world );

      /*
       * osc message layout
       * app/plab/chilitags/app/plab_chilitags.cpp
       * TODO google protocol buffers !!!
       */
      tag_msg.push_back( "id" );
      tag_msg.push_back( "size" );
      tag_msg.push_back( "corner_0x" );
      tag_msg.push_back( "corner_0y" );
      tag_msg.push_back( "corner_1x" );
      tag_msg.push_back( "corner_1y" );
      tag_msg.push_back( "corner_2x" );
      tag_msg.push_back( "corner_2y" );
      tag_msg.push_back( "corner_3x" );
      tag_msg.push_back( "corner_3y" );
      tag_msg.push_back( "translation_x" );
      tag_msg.push_back( "translation_y" );
      tag_msg.push_back( "translation_z" );
    };

    virtual void added(Entity &e) 
    {
      receiver.setup(tags_receiver_m.get(e)->port);
    }; 

    virtual void processEntity(Entity &e) 
    {
      TagsReceiverComponent* tags_receiver_data = tags_receiver_m.get(e);

      vector<Tag>& tags = tags_receiver_data->tags;

      while ( receiver.hasWaitingMessages() )
      {
        ofxOscMessage m;
        receiver.getNextMessage( &m );

        if ( m.getAddress() != "/tags" )
          continue;

        // there's a new tag list
        tags.clear();

        int ntags = m.getArgAsInt32( 0 );
        for ( int i = 0; i < ntags; i++ )
          tags.push_back( make_tag( i, m ) );
      }
    }; 

    virtual void renderEntity(Entity &e)
    {};

  private:

    ofxOscReceiver receiver;
    vector<string> tag_msg; //message layout

    ComponentMapper<TagsReceiverComponent> tags_receiver_m;

    // see osc message layout
    Tag make_tag( int i, ofxOscMessage& m )
    { 
      Tag tag;

      tag.id = m.getArgAsString( idx("id",i) );
      tag.size = m.getArgAsFloat( idx("size",i) ); 

      tag.corners.push_back( ofVec2f(
        m.getArgAsFloat( idx("corner_0x",i) ),
        m.getArgAsFloat( idx("corner_0y",i) )
      ));

      tag.corners.push_back( ofVec2f(
        m.getArgAsFloat( idx("corner_1x",i) ),
        m.getArgAsFloat( idx("corner_1y",i) )
      ));

      tag.corners.push_back( ofVec2f(
        m.getArgAsFloat( idx("corner_2x",i) ),
        m.getArgAsFloat( idx("corner_2y",i) )
      ));

      tag.corners.push_back( ofVec2f(
        m.getArgAsFloat( idx("corner_3x",i) ),
        m.getArgAsFloat( idx("corner_3y",i) )
      ));

      tag.translation.set(
        m.getArgAsFloat( idx("translation_x",i) ),
        m.getArgAsFloat( idx("translation_y",i) ),
        m.getArgAsFloat( idx("translation_z",i) )
      );

      //ofLogNotice("TagsReceiverSystem") << "\t\t" << "make tag ( idx " << i << " ) " << tag.str(); 

      return tag;
    };

    int idx( string tag_field, int tag_idx )
    { 
      int tlen = tag_msg.size();
      int header = 1; //tags length
      int i = std::find( tag_msg.begin(), tag_msg.end(), tag_field ) - tag_msg.begin();
      return i < 0 || i >= tlen ? -1 
        : i + (tag_idx * tlen + header);
    };

};

