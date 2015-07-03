#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "bloqs/Bloq.h"

using namespace artemis;

class TagsProcessingSystem : public ECSsystem 
{ 

  public:

    TagsProcessingSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<TagsProcessingComponent>();
      addComponentType<TagsReceiverComponent>();
    };

    virtual void initialize() 
    {
      tags_processing_m.init( *world );
      tags_receiver_m.init( *world );
      entity = NULL;
    };

    virtual void added(Entity &e) 
    {
      if ( entity != NULL ) 
      {
        ofLogWarning("TagsProcessingSystem") << "already inited";
        return;
      }
      entity = &e;

      TagsProcessingComponent* tags_proc_data = tags_processing_m.get(e);

      load_extrinsics( tags_proc_data->calib_stereo_file, calib_stereo );
      load_intrinsics( tags_proc_data->calib_rgb_file, calib_rgb );
      load_intrinsics( tags_proc_data->calib_depth_file, calib_depth );

      rgb_size = calib_rgb.getDistortedIntrinsics().getImageSize();
    }; 

    virtual void removed(Entity &e) 
    {
      entity = NULL;
    };

    virtual void processEntity(Entity &e) 
    {
      TagsProcessingComponent* tags_proc_data = tags_processing_m.get(e);
      TagsReceiverComponent* tags_receiver_data = tags_receiver_m.get(e);

      TS_START("TagsProcessingSystem");

      vector< shared_ptr<Bloq> >& bloqs = tags_proc_data->bloqs;
      vector<Tag>& tags = tags_receiver_data->tags;

      //TODO refactor BloqEventsComponent.h => events/BloqEvents.h 
      BloqEventsComponent* events = require_component<BloqEventsComponent>("core");

      int w = rgb_size.width;
      int h = rgb_size.height;

      //ofLogNotice("TagsProcessingSystem") << "tags " << tags.size();

      int len = tags.size();
      for ( int i = 0; i < len; i++ )
      {
        Tag& tag = tags[i];
        string id = tag.id;
        Bloq* bloq = get_bloq( id, bloqs );

        //ofLogNotice("TagsProcessingSystem") << "\t get bloq " << id << ": " << (bloq != NULL);

        if ( bloq == NULL )
        {
          shared_ptr<Bloq> _bloq( new Bloq() );
          update_bloq( _bloq.get(), tag, w, h ); 
          bloqs.push_back( _bloq );
          ofNotifyEvent( events->added, *_bloq );
        }

        else if ( update_bloq( bloq, tag, w, h ) )
        {
          ofNotifyEvent( events->updated, *bloq );
        }
      } 

      remove_bloqs( events, bloqs, tags );

      //ofLogNotice("TagsProcessingSystem") << "\t bloqs " << bloqs.size();

      TS_STOP("TagsProcessingSystem");
    }; 

    virtual void renderEntity(Entity &e)
    {
      TagsProcessingComponent* tags_proc_data = tags_processing_m.get(e);
      TagsReceiverComponent* tags_receiver_data = tags_receiver_m.get(e);

      if ( !tags_proc_data->render )
        return;

      vector< shared_ptr<Bloq> >& bloqs = tags_proc_data->bloqs;
      vector<Tag>& tags = tags_receiver_data->tags;

      RenderComponent* render_data = require_component<RenderComponent>("output");
      int dst_w = render_data->width;
      int dst_h = render_data->height;

      // render bloqs loc/angle
      ofPushStyle();
      ofSetLineWidth( 5 ); 
      ofSetColor(ofColor::red);
      ofVec2f loc;

      for (int i = 0; i < bloqs.size(); i++)
      {
        string id = bloqs[i]->id;
        ofVec2f& dir = bloqs[i]->dir; 
        loc.set( bloqs[i]->loc ); 

        loc.x *= dst_w;
        loc.y *= dst_h;

        ofLine( loc, loc + dir * 40 );
        ofRect( loc, 8, 8 );
        ofDrawBitmapString( id, loc ); 
      }
      ofPopStyle();

      // render tags 2d
      ofPushStyle();
      ofSetColor(ofColor::blue);
      for (int i = 0; i < tags.size(); i++)
        render_tag_2d( tags[i], dst_w, dst_h );
      ofPopStyle();
    };

  private:

    typedef TagsReceiverComponent::Tag Tag;

    Entity* entity;

    ComponentMapper<TagsProcessingComponent> tags_processing_m;
    ComponentMapper<TagsReceiverComponent> tags_receiver_m;

    Extrinsics calib_stereo;
    ofxCv::Calibration calib_rgb;
    ofxCv::Calibration calib_depth;
    cv::Size rgb_size;

    ofVec2f up2;
    ofVec3f up3;


    // tags

    Tag* get_tag( string id, vector<Tag>& tags )
    {
      int tags_len = tags.size();
      for ( size_t i = 0; i < tags_len; i++ )
        if ( tags[i].id == id )
          return &(tags[i]);
      return NULL;
    }; 


    // bloqs

    Bloq* get_bloq( string id, vector< shared_ptr<Bloq> >& bloqs )
    {
      for ( size_t i = 0; i < bloqs.size(); i++ )
        if ( bloqs[i]->id == id )
          return bloqs[i].get();
      return NULL;
    };

    bool update_bloq( Bloq* bloq, const Tag& tag, int w, int h )
    {
      ofVec2f tloc, tdir;

      //3d loc
      tag_loc_on_depth( tag, w, h, tloc );

      //debug: project back on rgb
      //ofVec2f tloc_rgb;
      //project( calib_rgb.getDistortedIntrinsics(), tag.translation, tloc_rgb );

      //2d loc
      //vector<ofVec2f> corners(4,ofVec2f());
      //tag_corners_normalized(tag, w, h, corners);
      //tag_loc_normalized(corners, tloc); 

      //if ( bloq->loc == tloc )
        //return false;

      //ofLog() << "update bloq " << tag.id;

      //3d angle/dir
      //ofVec3f axis;
      //float radians = tag_angle_axis(tag,axis);
      //tag_dir_from_angle(radians, tdir);   

      //2d angle/dir
      vector<ofVec2f> corners(4,ofVec2f());
      tag_corners_normalized( tag, w, h, corners );
      tag_dir_from_corners( corners, tdir );
      float radians = tag_angle_2d( tdir );

      //ofLog()
          //<< " angle deg " 
          //<< (radians*RAD_TO_DEG)
          //<< ", dir " << tdir;

      bloq->loc.set( tloc );
      bloq->dir.set( tdir );
      bloq->radians = radians;
      bloq->id = tag.id; 

      return true;
    };

    void remove_bloqs( BloqEventsComponent* events, vector< shared_ptr<Bloq> >& bloqs, vector<Tag>& tags )
    {
      vector< vector< shared_ptr<Bloq> >::iterator> to_remove;

      for ( int i = 0; i < bloqs.size(); i++ )
      {
        string id = bloqs[i]->id;
        if ( get_tag( id, tags ) != NULL ) 
          continue;
        to_remove.push_back( bloqs.begin() + i );

        ofNotifyEvent( events->removed, id );
      }

      for (size_t i = 0; i < to_remove.size(); i++)
        bloqs.erase( to_remove[i] );

    };


    //3d

    void transform_to_depth( const Tag& tag, ofVec3f& p3 )
    {
      cv::Mat T(3,1,CV_32FC1);
      T.at<float>(0,0) = tag.translation.x;
      T.at<float>(1,0) = tag.translation.y;
      T.at<float>(2,0) = tag.translation.z;

      T = calib_stereo.R * T + calib_stereo.T; 
      p3.x = T.at<float>(0,0);
      p3.y = T.at<float>(1,0);
      p3.z = T.at<float>(2,0);
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


    //2d

    //normalized [0,1] loc
    void tag_loc_on_depth( const Tag& tag, int w, int h, ofVec2f& tloc )
    {
      ofVec3f p3; 
      transform_to_depth( tag, p3 );
      project( calib_depth.getDistortedIntrinsics(), p3, tloc ); 
      tloc.x /= w;
      tloc.y /= h;
    }; 

    //normalized [0,1] loc
    void tag_loc_normalized( const vector<ofVec2f>& corners, ofVec2f& tloc )
    {
      for ( int i = 0; i < corners.size(); i++ )
      {
        tloc.x += corners[i].x;
        tloc.y += corners[i].y;
      }
      tloc.x /= 4.;
      tloc.y /= 4.;
    };

    //normalized [0,1] corners
    void tag_corners_normalized( const Tag& tag, int w, int h, vector<ofVec2f>& corners )
    {
      for ( int i = 0; i < tag.corners.size(); i++ )
        corners[i].set( tag.corners[i].x / w, tag.corners[i].y / h ); 
    };

    //normalized [0,1] dir
    void tag_dir_from_corners( const vector<ofVec2f>& corners, ofVec2f& tdir )
    {
      ofVec2f c0,c1;
      //ofVec2f c2,c3;

      c0.set(corners[0].x, corners[0].y);
      c1.set(corners[1].x, corners[1].y);
      //c2.set(corners[2].x, corners[2].y);
      //c3.set(corners[3].x, corners[3].y);

      tdir.set( c0 - c1 );
      tdir.normalize();
    };

    float tag_angle_2d( const ofVec2f& tdir )
    {
      return ( tdir.angleRad( up2 ) + PI ); //[0,PI]
    };


    //TODO tag.rotation
    //void tag_modelview( const Tag& tag, ofMatrix4x4& ofMV )
    //{
      ////opengl col-major format
      ////ofMV = ofxCv::makeMatrix(tag.rotation, tag.translation);

      ////cv::Mat T(3,1,CV_32FC1);
      //cv::Mat T;
      //T.at<float>(0,0) = tag.translation.x;
      //T.at<float>(1,0) = tag.translation.y;
      //T.at<float>(2,0) = tag.translation.z;

      //// opencv row-major format
      //cv::Mat R(3,3,CV_32FC1);
      //cv::Rodrigues( tag.rotation, R );

      //cv::Mat cvMV = cv::Mat::eye(4,4,CV_32FC1);
      ////rotation
      //for (int i=0;i<3;i++)
        //for (int j=0;j<3;j++)
          //cvMV.at<float>(i,j) = R.at<float>(i,j);
      ////translation
      //for (int i=0;i<3;i++)
        //cvMV.at<float>(i,3) = T.at<float>(i,0);
      ////cvMV.inv();

      ////copy
      //for (int i=0;i<4;i++)
        //for (int j=0;j<4;j++)
          //ofMV(i,j) = cvMV.at<float>(i,j);

      //ofMV = ofMatrix4x4::getTransposedOf( ofMV );
      ////ofMV.scale( 1, 1, -1 );
    //};

    // opencv row-major format
    //void tag_rotation_axis( const Tag& tag, ofVec3f& X, ofVec3f& Y, ofVec3f& Z )
    //{
      ////TODO tag.rotation
      //cv::Mat R(3,3,CV_32FC1);
      //cv::Rodrigues( tag.rotation, R );

      //X.set(
          //R.at<float>(0,0),
          //R.at<float>(1,0),
          //R.at<float>(2,0)
          //);

      //Y.set(
          //R.at<float>(0,1),
          //R.at<float>(1,1),
          //R.at<float>(2,1)
          //);

      //Z.set(
          //R.at<float>(0,2),
          //R.at<float>(1,2),
          //R.at<float>(2,2)
          //);
    //};

    //float tag_angle_axis( const Tag& tag, ofVec3f& axis )
    //{
      //ofMatrix4x4 mv;
      //tag_modelview( tag, mv );
      //ofQuaternion q = mv.getRotate();
      //float angle_deg;
      //q.getRotate( angle_deg, axis );

      ////ofLog() << " -- calc angle"
        ////<< ", mv: \n" << mv << "\n"
        ////<< ", deg: " << angle_deg
        ////<< ", axis: " << axis;

      //return angle_deg * DEG_TO_RAD;
    //};

    void tag_dir_from_angle( float radians, ofVec2f& tdir )
    {
      tdir.set( up2 );
      tdir.rotate( radians * RAD_TO_DEG );
    };

    void render_tag_2d( Tag& tag, int dst_w, int dst_h )
    {
      int w = rgb_size.width;
      int h = rgb_size.height;
      vector<ofVec2f> corners(4,ofVec2f());
      tag_corners_normalized(tag, w, h, corners);

      ofVec2f dst_s( dst_w, dst_h );

      ofVec2f p0,p1;
      ofPoint ctr(0,0);
      for ( int i = 0; i < corners.size(); i++ )
      {
        p0 = corners[i] * dst_s;
        p1 = corners[ (i+1)%4 ] * dst_s;
        ofLine( p0.x, p0.y, p1.x, p1.y );
        ctr.x += p0.x;
        ctr.y += p0.y;
      }

      ctr.x /= 4.;
      ctr.y /= 4.;
      ctr *= dst_s;
      ofDrawBitmapString( tag.id, ctr );
    }


    // calibration

    void load_extrinsics( string filename, Extrinsics& calib_stereo )
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
      float scale = 1.;

      calib_stereo.T *= scale;

      //ofLogNotice("TagsProcessingSystem")
        //<< "load_extrinsics:"
        //<< "\n" << "T: \n" << calib_stereo.T
        //<< "\n" << "R: \n" << calib_stereo.R
        //<< "\n";
    };

    void load_intrinsics( string filename, ofxCv::Calibration& calib ) 
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

