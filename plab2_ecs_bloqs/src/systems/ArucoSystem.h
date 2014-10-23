#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "bloqs/BloqAruco.h"
#include "components/BloqEventsComponent.h"
#include "components/ArucoComponent.h"
#include "components/RGBComponent.h"

using namespace artemis;

class ArucoSystem : public ECSsystem 
{ 

  public:

    ArucoSystem() 
    {
      addComponentType<BloqEventsComponent>();
      addComponentType<ArucoComponent>();
      addComponentType<RGBComponent>();
    };

    virtual void initialize() 
    {
      bloq_events_m.init( *world );
      aruco_m.init( *world );
      rgb_m.init( *world );
    };

    virtual void added(Entity &e) 
    {
      RGBComponent* rgb = rgb_m.get(e);

      int w = rgb->width;
      int h = rgb->height;
      this->channels = 3;

      rgb_pix.allocate( w, h, channels );
      rgb_pix.set(0);

      //no calibration, no board config
      aruco.setup2d( w, h );
      //aruco.setThresholdMethod(aruco::MarkerDetector::CANNY);

    };

    // entity: game
    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("ArucoSystem") << "process entity " << e.getId();

      BloqEventsComponent* events = bloq_events_m.get(e);
      RGBComponent* rgb = rgb_m.get(e);

      if ( rgb->dirty )
        update( rgb->color_pix, rgb->width, rgb->height, events );

      // TODO move to render system
      render(); 
    };

  private: 

    ComponentMapper<BloqEventsComponent> bloq_events_m;
    ComponentMapper<ArucoComponent> aruco_m;
    ComponentMapper<RGBComponent> rgb_m;


    ofxAruco aruco;
    ofPixels rgb_pix; 
    int channels;
    vector< shared_ptr<BloqAruco> > bloqs;


    void update( uint8_t *pixels, int w, int h, BloqEventsComponent* events )
    {
      rgb_pix.setFromPixels( pixels, w, h, channels );

      aruco.detectMarkers( rgb_pix );

      vector<aruco::Marker>& markers = aruco.getMarkers();

      for ( int i = 0; i < markers.size(); i++ )
      {

        aruco::Marker& m = markers[i];
        string bloq_id = ofToString( m.idMarker );
        BloqAruco* bloq = get_bloq( bloq_id );

        if ( bloq == NULL )
        {
          shared_ptr<BloqAruco> _bloq( new BloqAruco( m ) );
          bloqs.push_back( _bloq );
          ofNotifyEvent( events->added, *_bloq );
        }

        else if ( bloq->update( m ) )
        {
          ofNotifyEvent( events->updated, *bloq );
        }
      }

      remove_bloqs_missing( markers, events );
    };

    void render()
    {
      ofSetLineWidth( 3 );
      aruco.draw2d();
    }; 

    BloqAruco* get_bloq( string id )
    {
      for( size_t i = 0; i < bloqs.size(); i++ )
        if ( bloqs[i]->id == id )
          return bloqs[i].get();
      return NULL;
    };

    aruco::Marker* get_marker( int id, vector<aruco::Marker>& markers )
    {
      for( size_t i = 0; i < markers.size(); i++ )
        if ( markers[i].idMarker == id )
          return &markers[i];
      return NULL;
    }; 

    void remove_bloqs_missing( vector<aruco::Marker>& markers, BloqEventsComponent* events )
    {

      vector< vector< shared_ptr<BloqAruco> >::iterator> to_remove;

      for ( int i = 0; i < bloqs.size(); i++ )
      {
        string bloq_id = bloqs[i]->id;
        int marker_id = ofToInt( bloq_id );
        aruco::Marker* m = get_marker( marker_id, markers );
        if ( m != NULL ) continue;
        to_remove.push_back( bloqs.begin() + i );

        ofNotifyEvent( events->removed, bloq_id );
      }

      for (size_t i = 0; i < to_remove.size(); i++)
        bloqs.erase( to_remove[i] );

    };

};

