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

      id = ofToString( m.idMarker ); 
      loc = get_center( m );

      p0 = ofVec2f( m[0].x, m[0].y );
      p1 = ofVec2f( m[1].x, m[1].y );
      p2 = ofVec2f( m[2].x, m[2].y );
      p3 = ofVec2f( m[3].x, m[3].y );

      angle = p0.angleRad( p1 );

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

