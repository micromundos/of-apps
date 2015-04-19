#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "Bloq.h"
#include "ofxAruco.h"
#include "ofxCv.h"

using namespace artemis;

struct Extrinsics
{
  cv::Mat R, T;
};

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
      depth_m.init( *world );
      inited = false; 
      up2 = ofVec2f(0,1);
      up3 = ofVec3f(0,1,0);
    };

    virtual void added(Entity &e) 
    {
      if (inited) 
      {
        ofLogWarning("ArucoSystem") << "aruco is already inited";
        return;
      }
      inited = true;

      ArucoComponent* aruco_data = aruco_m.get(e);
      RgbComponent* rgb_data = rgb_m.get(e);
      DepthComponent* depth_data = depth_m.get(e);

      int w = rgb_data->width;
      int h = rgb_data->height;

      load_calib_stereo( aruco_data->calib_stereo_file, calib_stereo );
      load_calib( aruco_data->calib_rgb_file, calib_rgb );
      load_calib( aruco_data->calib_depth_file, calib_depth );

      //this->channels = rgb_data->ir ?1:3;
      this->channels = 3;

      rgb_pix.allocate( w, h, channels );
      rgb_pix.set(0); 

      //aruco.setup2d( w, h ); //no calibration, no board config
      aruco.setupXML( aruco_data->calib_rgb_file, w, h );
      //aruco.setThresholdMethod(aruco::MarkerDetector::CANNY);
    };

    virtual void processEntity(Entity &e) 
    {
      ArucoComponent* aruco_data = aruco_m.get(e);
      RgbComponent* rgb_data = rgb_m.get(e);
      DepthComponent* depth_data = depth_m.get(e);

      if ( !rgb_data->dirty )
        return;

      BloqEventsComponent* events = require_component<BloqEventsComponent>("core");

      uint8_t *pixels = rgb_data->color_pix;
      int w = rgb_data->width;
      int h = rgb_data->height;

      rgb_pix.setFromPixels( pixels, w, h, channels );
      aruco.detectMarkers( rgb_pix );
      vector<aruco::Marker>& markers = aruco.getMarkers();

      //ofLog() << "aruco update" 
        //<< ", markers: " << markers.size();

      for ( int i = 0; i < aruco.getNumMarkers(); i++ )
      {
        aruco::Marker& m = markers[i];
        string bloq_id = ofToString( m.idMarker );
        Bloq* bloq = get_bloq(bloq_id);

        if ( bloq == NULL )
        {
          shared_ptr<Bloq> _bloq( new Bloq() );
          update_bloq( _bloq.get(), m, w, h ); 
          bloqs.push_back( _bloq );
          ofNotifyEvent( events->added, *_bloq );
        }

        else if (update_bloq( bloq, m, w, h ))
        {
          ofNotifyEvent( events->updated, *bloq );
        }
      }

      remove_bloqs_missing( events );
    }; 

    virtual void renderEntity(Entity &e)
    {

      ArucoComponent* aruco_data = aruco_m.get(e);
      RgbComponent* rgb_data = rgb_m.get(e);

      //debug
      if ( !aruco_data->render )
        return;

      RenderComponent* render_data = require_component<RenderComponent>("output");

      //aruco.draw2d();
      //aruco.draw3d();

      ofPushStyle();
      ofSetLineWidth( 5 ); 

      ofSetColor(ofColor::green);
      ofVec2f loc;
      for (int i = 0; i < bloqs.size(); i++)
      {
        string id = bloqs[i]->id;
        ofVec2f& dir = bloqs[i]->dir; 
        loc.set( bloqs[i]->loc ); 

        //loc.x *= rgb_data->width;
        //loc.y *= rgb_data->height;
        loc.x *= render_data->width;
        loc.y *= render_data->height;

        ofLine( loc, loc + dir * 40 );
        ofRect( loc, 8, 8 );
        ofDrawBitmapString( id, loc ); 
      }

      ofPopStyle();
    };

  private: 

    ComponentMapper<ArucoComponent> aruco_m;
    ComponentMapper<RgbComponent> rgb_m;
    ComponentMapper<DepthComponent> depth_m;

    vector< shared_ptr<Bloq> > bloqs; //move to ArucoComponent?

    ofxAruco aruco;
    ofPixels rgb_pix; 
    int channels;
    bool inited;

    Extrinsics calib_stereo;
    ofxCv::Calibration calib_rgb;
    ofxCv::Calibration calib_depth;

    ofVec2f up2;
    ofVec3f up3;

    Bloq* get_bloq( string id )
    {
      for( size_t i = 0; i < bloqs.size(); i++ )
        if ( bloqs[i]->id == id )
          return bloqs[i].get();
      return NULL;
    };

    aruco::Marker* get_marker( int id )
    {
      vector<aruco::Marker>& markers = aruco.getMarkers();
      for( size_t i = 0; i < aruco.getNumMarkers(); i++ )
        if ( markers[i].idMarker == id )
          return &markers[i];
      return NULL;
    }; 

    void remove_bloqs_missing( BloqEventsComponent* events )
    {
      vector<aruco::Marker>& markers = aruco.getMarkers();
      vector< vector< shared_ptr<Bloq> >::iterator> to_remove;

      for ( int i = 0; i < bloqs.size(); i++ )
      {
        string bloq_id = bloqs[i]->id;
        int marker_id = ofToInt( bloq_id );
        if ( get_marker( marker_id ) != NULL ) 
          continue;
        to_remove.push_back( bloqs.begin() + i );

        ofNotifyEvent( events->removed, bloq_id );
      }

      for (size_t i = 0; i < to_remove.size(); i++)
        bloqs.erase( to_remove[i] );

    };

    //2d

    //normalized [0,1] corners
    void marker_corners_on_rgb(const aruco::Marker& m, int w, int h, vector<ofVec2f>& corners )
    {
      for ( int i = 0; i < m.size(); i++ )
        corners[i].set( m[i].x / w, m[i].y / h ); 
    };

    //normalized [0,1] loc
    void marker_loc_on_rgb( const vector<ofVec2f>& corners, ofVec2f& mloc )
    {
      for ( int i = 0; i < corners.size(); i++ )
      {
        mloc.x += corners[i].x;
        mloc.y += corners[i].y;
      }
      mloc.x /= 4.;
      mloc.y /= 4.;
    };

    void marker_dir_from_corners( const vector<ofVec2f>& corners, ofVec2f& mdir )
    {
      ofVec2f c0,c1;
      //ofVec2f c2,c3;

      c0.set(corners[0].x, corners[0].y);
      c1.set(corners[1].x, corners[1].y);
      //c2.set(corners[2].x, corners[2].y);
      //c3.set(corners[3].x, corners[3].y);

      mdir.set( c0 - c1 );
      mdir.normalize();
    };

    float marker_angle_2d( const ofVec2f& mdir )
    {
      return ( mdir.angleRad( up2 ) + PI ); //[0,PI]
    };

    //3d

    void transform_to_depth( const cv::Mat& marker_T, float marker_size, ofVec3f& p3 )
    {
      cv::Mat mT = calib_stereo.R * marker_T + calib_stereo.T; 
      p3.x = mT.at<float>(0,0) - marker_size * 0.5;
      p3.y = mT.at<float>(1,0) - marker_size * 0.5;
      p3.z = mT.at<float>(2,0);
    };

    void project( const ofxCv::Intrinsics& intrinsics, const ofVec3f& p3, ofVec2f& p2 )
    {
      cv::Mat cameraMatrix = intrinsics.getCameraMatrix();

      float fx = cameraMatrix.at<double>(0, 0);
      float fy = cameraMatrix.at<double>(1, 1);
      float cx = cameraMatrix.at<double>(0, 2);
      float cy = cameraMatrix.at<double>(1, 2);

      //cv::Point2d principalPoint = intrinsics.getPrincipalPoint();
      //float cx = principalPoint.x;
      //float cy = principalPoint.y;

      p2.x = (p3.x * fx / p3.z) + cx;
      p2.y = (p3.y * fy / p3.z) + cy;
    };

    void marker_modelview( const aruco::Marker& m, ofMatrix4x4& ofMV )
    {
      //opengl col-major format
      //ofMV = ofxCv::makeMatrix( m.Rvec, m.Tvec );

      // opencv row-major format
      cv::Mat mR(3,3,CV_32FC1);
      cv::Rodrigues( m.Rvec, mR );

      cv::Mat cvMV = cv::Mat::eye(4,4,CV_32FC1);
      //rotation
      for (int i=0;i<3;i++)
        for (int j=0;j<3;j++)
          cvMV.at<float>(i,j) = mR.at<float>(i,j);
      //translation
      for (int i=0;i<3;i++)
        cvMV.at<float>(i,3) = m.Tvec.at<float>(i,0);
      //cvMV.inv();

      //copy
      for (int i=0;i<4;i++)
        for (int j=0;j<4;j++)
          ofMV(i,j) = cvMV.at<float>(i,j);

      ofMV = ofMatrix4x4::getTransposedOf( ofMV );
      //ofMV.scale( 1, 1, -1 );
    };

    // opencv row-major format
    void marker_rotation_axis( const aruco::Marker& m, ofVec3f& X, ofVec3f& Y, ofVec3f& Z )
    {
      cv::Mat mR(3,3,CV_32FC1);
      cv::Rodrigues( m.Rvec, mR );

      X.set(
          mR.at<float>(0,0),
          mR.at<float>(1,0),
          mR.at<float>(2,0)
          );

      Y.set(
          mR.at<float>(0,1),
          mR.at<float>(1,1),
          mR.at<float>(2,1)
          );

      Z.set(
          mR.at<float>(0,2),
          mR.at<float>(1,2),
          mR.at<float>(2,2)
          );
    };

    float marker_angle_axis( const aruco::Marker& m, ofVec3f& axis )
    {
      ofMatrix4x4 mv;
      marker_modelview(m, mv);
      ofQuaternion q = mv.getRotate();
      float angle_deg;
      q.getRotate( angle_deg, axis );

      //ofLog() << " -- calc angle"
        //<< ", mv: \n" << mv << "\n"
        //<< ", deg: " << angle_deg
        //<< ", axis: " << axis;

      return angle_deg * DEG_TO_RAD;
    };

    void marker_dir_from_angle( float radians, ofVec2f& mdir )
    {
      mdir.set( up2 );
      mdir.rotate( radians * RAD_TO_DEG );
    };

    //normalized [0,1] loc
    void marker_loc_on_depth( const cv::Mat& marker_T, float marker_size, int w, int h, ofVec2f& mloc )
    {
      ofVec3f p3; 
      transform_to_depth(marker_T, marker_size, p3);
      project( calib_depth.getDistortedIntrinsics(), p3, mloc ); 
      mloc.x /= w;
      mloc.y /= h;
    };

    void debug_marker_loc_on_rgb( const cv::Mat& marker_T, ofVec2f& mloc_rgb )
    {
      ofVec3f mT;
      mT.x = marker_T.at<float>(0,0);
      mT.y = marker_T.at<float>(1,0);
      mT.z = marker_T.at<float>(2,0);
      project( calib_rgb.getDistortedIntrinsics(), mT, mloc_rgb );
    };

    bool update_bloq( Bloq* bloq, const aruco::Marker& m, int w, int h )
    {
      ofVec2f mloc, mdir;

      //3d loc
      marker_loc_on_depth( m.Tvec.clone(), m.ssize, w, h, mloc );
      //debug: project back on rgb
      //ofVec2f mloc_rgb;
      //debug_marker_loc_on_rgb(m, mloc_rgb);

      //2d loc
      //vector<ofVec2f> corners(4,ofVec2f());
      //marker_corners_on_rgb(m, w, h, corners);
      //marker_loc_on_rgb(corners, mloc); 

      if ( bloq->loc == mloc )
        return false;

      //ofLog() << "update bloq " << m.idMarker;

      //3d angle/dir
      //ofVec3f axis;
      //float radians = marker_angle_axis(m, axis);
      //marker_dir_from_angle(radians, mdir);   

      //2d angle/dir
      vector<ofVec2f> corners(4,ofVec2f());
      marker_corners_on_rgb(m, w, h, corners);
      marker_dir_from_corners(corners, mdir);
      float radians = marker_angle_2d(mdir);

      //ofLog()
          //<< " angle deg " << (radians*RAD_TO_DEG)
          //<< ", dir " << mdir;

      bloq->loc.set( mloc );
      bloq->dir.set( mdir );
      bloq->radians = radians;
      bloq->id = ofToString( m.idMarker ); 
      return true;
    }; 


    void load_calib_stereo( string filename, Extrinsics& calib_stereo )
    {
      bool absolute = false;

      cv::FileStorage fs( ofToDataPath(filename, absolute), cv::FileStorage::READ );

      fs["R"] >> calib_stereo.R;
      fs["T"] >> calib_stereo.T;

      if ( calib_stereo.R.type() != CV_32FC1 ) 
        calib_stereo.R.convertTo( calib_stereo.R, CV_32FC1 );

      if ( calib_stereo.T.type() != CV_32FC1 ) 
        calib_stereo.T.convertTo( calib_stereo.T, CV_32FC1 );

      //@#$%ˆ& scale.....
      float scale = 0.01;

      calib_stereo.T *= scale;

      ofLogNotice("ArucoSystem")
        << "load_calib_stereo:"
        << "\n" << "T: \n" << calib_stereo.T
        << "\n" << "R: \n" << calib_stereo.R
        << "\n";
    };

    void load_calib( string filename, ofxCv::Calibration& calib ) 
    {
      bool absolute = false;

      cv::FileStorage fs( ofToDataPath(filename, absolute), cv::FileStorage::READ );

      cv::Size imageSize, sensorSize;
      cv::Mat cameraMatrix;

      cv::Mat distCoeffs;
      float reprojectionError;

      fs["camera_matrix"] >> cameraMatrix;
      fs["image_width"] >> imageSize.width;
      fs["image_height"] >> imageSize.height;
      fs["sensor_width"] >> sensorSize.width;
      fs["sensor_height"] >> sensorSize.height;
      fs["distortion_coefficients"] >> distCoeffs;
      fs["reprojection_error"] >> reprojectionError;

      ofxCv::Intrinsics intrinsics;
      intrinsics.setup( cameraMatrix, imageSize, sensorSize ); 
      calib.setIntrinsics( intrinsics, distCoeffs ); //fires calib.updateUndistortion();
    };

};

