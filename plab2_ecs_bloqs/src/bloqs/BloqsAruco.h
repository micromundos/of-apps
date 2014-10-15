#pragma once

#include "ofMain.h"
#include "ofxAruco.h"
#include "bloqs/Bloq.h"

class BloqAruco : public Bloq
{
  public:

    BloqAruco( aruco::Marker& m )
    {
      update( m );
    };
    ~BloqAruco(){};

    bool update( aruco::Marker& m )
    {
      if (!diff_loc( m ))
        return false;

      id = m.idMarker; 
      loc = get_center( m );

      p0 = ofVec2f( m[0].x, m[0].y );
      p1 = ofVec2f( m[1].x, m[1].y );
      p2 = ofVec2f( m[2].x, m[2].y );
      p3 = ofVec2f( m[3].x, m[3].y );

      //TODO testear y mejorar esto
      angle = p0.angle( p1 );

      return true;
    };

    bool diff_loc( aruco::Marker& m )
    {
      return loc != get_center(m);
    };

    ofVec2f p0,p1,p2,p3;

  private:

    ofVec2f get_center(const aruco::Marker& marker)
    {
      ofVec2f ctr(0,0);
      for ( int i = 0; i < 4; i++ )
      {
        ctr.x += marker[i].x;
        ctr.y += marker[i].y;
      }
      ctr.x /= 4.;
      ctr.y /= 4.;
      return ctr;
    };
};

/*
 * bloqs manager
 */

class BloqsAruco
{
  public:

    BloqsAruco(){};
    ~BloqsAruco(){};

    void init( int w, int h )
    {
      //no calibration, no board config
      aruco.setup2d( w, h );
      //aruco.setThresholdMethod(aruco::MarkerDetector::CANNY);
    };

    void update( ofPixels &pixels )
    {
      aruco.detectMarkers( pixels );

      vector<aruco::Marker>& markers = aruco.getMarkers();

      for ( int i = 0; i < markers.size(); i++ )
      {

        aruco::Marker& m = markers[i];
        BloqAruco* bloq = get_bloq( m.idMarker );

        if ( bloq == NULL )
        {
          shared_ptr<BloqAruco> _bloq( new BloqAruco( m ) );
          bloqs.push_back( _bloq );
          ofNotifyEvent( added, *_bloq, this );
        }

        else if ( bloq->update( m ) )
        {
          ofNotifyEvent( updated, *bloq, this );
        }
      }

      remove_bloqs_missing( markers );

    };

    void render()
    {
      ofSetLineWidth( 3 );
      aruco.draw2d();
    };

    ofEvent<Bloq> added;
    ofEvent<Bloq> updated;
    ofEvent<int> removed;

  private:

    ofxAruco aruco;
    vector< shared_ptr<BloqAruco> > bloqs;

    BloqAruco* get_bloq( int id )
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

    void remove_bloqs_missing( vector<aruco::Marker>& markers )
    {
      vector< vector< shared_ptr<BloqAruco> >::iterator> to_remove;

      for ( int i = 0; i < bloqs.size(); i++ )
      {
        int bloq_id = bloqs[i]->id;
        aruco::Marker* m = get_marker( bloq_id, markers );
        if ( m != NULL ) continue;
        to_remove.push_back( bloqs.begin() + i );

        ofNotifyEvent( removed, bloq_id, this );
      }

      for (size_t i = 0; i < to_remove.size(); i++)
        bloqs.erase( to_remove[i] );

    };

};

