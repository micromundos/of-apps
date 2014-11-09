#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "bloqs/BloqAruco.h"
#include "components/Components.h"

using namespace artemis;

class ArucoSystem : public ECSsystem 
{ 

  public:

    ArucoSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<ArucoComponent>();
      addComponentType<RGBComponent>();
    };

    virtual void initialize() 
    {
      aruco_m.init( *world );
      rgb_m.init( *world );
      inited = false; 
    };

    virtual void added(Entity &e) 
    {
      //TODO mejorar
      RGBComponent* rgb = rgb_m.get(e);
      init( rgb->width, rgb->height );
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("ArucoSystem") << "process entity " << e.getId();

      RGBComponent* rgb = rgb_m.get(e);

      BloqEventsComponent* events = component<BloqEventsComponent>("core");

      if ( rgb->dirty )
        update( rgb->color_pix, rgb->width, rgb->height, events );

    }; 

    virtual void render()
    {
      RenderComponent* render_data = component<RenderComponent>("output");
      int w = render_data->width;
      int h = render_data->height;

      ofSetLineWidth( 5 );
      //aruco.draw2d();

      ofSetColor(ofColor::green);
      for (int i = 0; i<bloqs.size(); i++)
      {
        string id = bloqs[i]->id;
        ofVec2f loc( bloqs[i]->loc );
        loc.x *= w;
        loc.y *= h;
        ofVec2f& dir = bloqs[i]->dir;
        ofLine( loc, loc + dir * 40 );
        ofRect( loc, 8, 8 );
        ofDrawBitmapString( id, loc );
      }
    };

  private: 

    ComponentMapper<ArucoComponent> aruco_m;
    ComponentMapper<RGBComponent> rgb_m;

    ofxAruco aruco;
    ofPixels rgb_pix; 
    int channels;
    bool inited;
    vector< shared_ptr<BloqAruco> > bloqs;

    void init(int w, int h)
    {
      if (inited) return;

      this->channels = 3;

      rgb_pix.allocate( w, h, channels );
      rgb_pix.set(0); 

      //no calibration, no board config
      aruco.setup2d( w, h );
      //aruco.setThresholdMethod(aruco::MarkerDetector::CANNY);

      inited = true;
    };

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
          shared_ptr<BloqAruco> _bloq( new BloqAruco( m, w, h ) );
          bloqs.push_back( _bloq );
          ofNotifyEvent( events->added, *_bloq );
        }

        else if ( bloq->update(m, w, h) )
        {
          ofNotifyEvent( events->updated, *bloq );
        }
      }

      remove_bloqs_missing( markers, events );
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

