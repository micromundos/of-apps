#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "bloqs/Bloq.h"
#include "ecs/Components.h"
#include "ofxAruco.h"
#include "ofxCv.h"

using namespace artemis;

class ArucoSystem : public ECSsystem 
{ 

  public:

    ArucoSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<ArucoComponent>();
      addComponentType<RgbComponent>();
      addComponentType<DepthComponent>();
    };

    virtual void initialize() 
    {
      aruco_m.init( *world );
      rgb_m.init( *world );
      inited = false; 
    };

    virtual void added(Entity &e) 
    {
      init( rgb_m.get(e) );
    };

    virtual void processEntity(Entity &e) 
    {
      ArucoComponent* aruco_data = aruco_m.get(e);
      RgbComponent* rgb = rgb_m.get(e);

      BloqEventsComponent* events = require_component<BloqEventsComponent>("core");

      if ( rgb->dirty )
        update( rgb->color_pix, rgb->width, rgb->height, events, aruco_data );

    }; 

    virtual void render_entity(Entity &e)
    {

      ArucoComponent* aruco_data = aruco_m.get(e);

      if ( !aruco_data->render )
        return;

      RenderComponent* render_data = require_component<RenderComponent>("output");

      int w = render_data->width;
      int h = render_data->height;

      ofPushStyle();
      ofSetLineWidth( 5 );
      //aruco.draw2d();

      vector< shared_ptr<Bloq> >& bloqs = aruco_data->bloqs;

      ofSetColor(ofColor::green);
      for (int i = 0; i < bloqs.size(); i++)
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

      ofPopStyle();
    };

  private: 

    ComponentMapper<ArucoComponent> aruco_m;
    ComponentMapper<RgbComponent> rgb_m;

    ofxAruco aruco;
    ofPixels rgb_pix; 
    int channels;
    bool inited;

    void init( RgbComponent* rgb_data )
    {
      if (inited) 
      {
        ofLogWarning("ArucoSystem") << "aruco is already inited";
        return;
      }
      inited = true;

      int w = rgb_data->width;
      int h = rgb_data->height;
      string calib = rgb_data->calibration;

      //this->channels = rgb_data->ir ?1:3;
      this->channels = 3;

      rgb_pix.allocate( w, h, channels );
      rgb_pix.set(0); 

      //no calibration, no board config
      aruco.setup2d( w, h );
      //aruco.setThresholdMethod(aruco::MarkerDetector::CANNY);
    };

    void update( uint8_t *pixels, int w, int h, BloqEventsComponent* events, ArucoComponent* aruco_data )
    {
      vector< shared_ptr<Bloq> >& bloqs = aruco_data->bloqs;

      rgb_pix.setFromPixels( pixels, w, h, channels );

      aruco.detectMarkers( rgb_pix );

      vector<aruco::Marker>& markers = aruco.getMarkers();

      vector<ofVec2f> corners(4,ofVec2f());
      ofVec2f mloc, mdir;

      for ( int i = 0; i < markers.size(); i++ )
      {

        aruco::Marker& m = markers[i];
        string bloq_id = ofToString( m.idMarker );
        Bloq* bloq = get_bloq(bloq_id, aruco_data);

        if ( bloq == NULL )
        {
          shared_ptr<Bloq> _bloq( new Bloq() );
          _bloq->id = ofToString( m.idMarker ); 
          update_bloq( _bloq.get(), m, w, h, corners, mloc, mdir ); 
          bloqs.push_back( _bloq );
          ofNotifyEvent( events->added, *_bloq );
        }

        else if (update_bloq( bloq, m, w, h, corners, mloc, mdir ))
        {
          ofNotifyEvent( events->updated, *bloq );
        }
      }

      remove_bloqs_missing( markers, events, aruco_data );
    }; 

    Bloq* get_bloq( string id, ArucoComponent* aruco_data )
    {
      vector< shared_ptr<Bloq> >& bloqs = aruco_data->bloqs;

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

    void remove_bloqs_missing( vector<aruco::Marker>& markers, BloqEventsComponent* events, ArucoComponent* aruco_data )
    {

      vector< shared_ptr<Bloq> >& bloqs = aruco_data->bloqs;

      vector< vector< shared_ptr<Bloq> >::iterator> to_remove;

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

    //normalized [0,1] corners
    void marker_corners(const aruco::Marker& m, int w, int h, vector<ofVec2f>& corners )
    {
      for ( int i = 0; i < m.size(); i++ )
      {
        //TODO project_on_kinect()
        corners[i].set( m[i].x / w, m[i].y / h );
      }
    };

    void marker_loc( const vector<ofVec2f>& corners, ofVec2f& mloc )
    {
      for ( int i = 0; i < corners.size(); i++ )
      {
        mloc.x += corners[i].x;
        mloc.y += corners[i].y;
      }
      mloc.x /= 4.;
      mloc.y /= 4.;
    };

    void marker_dir( const vector<ofVec2f>& corners, ofVec2f& mdir )
    {
      ofVec2f p0,p1;
      //ofVec2f p2,p3;

      p0.set( corners[0].x, corners[0].y );
      p1.set( corners[1].x, corners[1].y );
      //p2.set( corners[2].x, corners[2].y );
      //p3.set( corners[3].x, corners[3].y );

      mdir.set( p0 - p1 );
      mdir.normalize();
    };

    bool update_bloq( Bloq* bloq, const aruco::Marker& m, int w, int h, vector<ofVec2f>& corners, ofVec2f& mloc, ofVec2f& mdir )
    {
      marker_corners(m,w,h, corners);
      marker_loc(corners, mloc);
      marker_dir(corners, mdir);
      if ( bloq->loc == mloc )
        return false;
      bloq->loc.set( mloc );
      bloq->dir.set( mdir );
      return true;
    };

    //void project_on_kinect( const ofVec3f& p3, ofVec2f& p2 )
    //{
      //const ofxCv::Intrinsics& _int = calib_kinect.getDistortedIntrinsics();
      ////const ofxCv::Intrinsics& _int = calib_kinect.getUndistortedIntrinsics();

      //cv::Mat cameraMatrix = _int.getCameraMatrix();
      //cv::Point2d principalPoint = _int.getPrincipalPoint();

      //float fx = cameraMatrix.at<double>(0, 0);
      //float fy = cameraMatrix.at<double>(1, 1);
      //float cx = principalPoint.x;
      //float cy = principalPoint.y;

      //p2.x = (p3.x * fx / p3.z) + cx;
      //p2.y = (p3.y * fy / p3.z) + cy;
    //};

};

