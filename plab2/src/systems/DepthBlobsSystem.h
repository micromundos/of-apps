
//==========
//DEPRECATED
//==========

#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "components/Components.h"
#include "ofxOpenCv.h"
//#include "ofxBlur.h"
#include "keys.h"

using namespace artemis;

class DepthBlobsSystem : public ECSsystem 
{ 

  public:

    DepthBlobsSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<DepthComponent>();
      addComponentType<BlobsComponent>();
    };

    virtual void initialize() 
    {
      blobs_m.init( *world );
      depth_m.init( *world ); 
      inited = false; 

      //TODO add params, rm keys
      ofAddListener( ofEvents().keyPressed, this, &DepthBlobsSystem::keyPressed );
    };

    virtual void added(Entity &e) 
    {
      //TODO mejorar init
      DepthComponent* depth = depth_m.get(e);
      init(depth->width, depth->height);
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("DepthBlobsSystem") << "process entity " << e.getId();

      DepthComponent* depth = depth_m.get(e);
      BlobsComponent* blobs_data = blobs_m.get(e);
      if ( depth->dirty )
        update( depth->depth_pix_grey, depth->width, depth->height, blobs_data );
 
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      for (int i=0;i<bag.getCount();i++)
        processEntity( *bag.get(i) );
    };

    virtual void render_entity(Entity &e)
    {
      RenderComponent* render_data = component<RenderComponent>("output");
      int w = render_data->width;
      int h = render_data->height;

      ofSetColor(255);
      //ofSetLineWidth(1);
      grey_img.draw(0, 0, w, h);
      //blur_img.draw(0, 0, w, h);
      contourFinder.draw(0, 0, w, h);
    };


    //TODO add params, rm keys
    void keyPressed(ofKeyEventArgs &args)
    {

      switch (args.key)
      { 
        
        case keys::blobs_thres_near_inc:
          threshold_near ++;
          if (threshold_near > 255) threshold_near = 255; 
          ofLogNotice("DepthBlobsSystem") << "threshold near " << threshold_near << " / far " << threshold_far;
          break;

        case keys::blobs_thres_near_dec:
          threshold_near --;
          if (threshold_near < 0) threshold_near = 0;
          ofLogNotice("DepthBlobsSystem") << "threshold near " << threshold_near << " / far " << threshold_far;
          break;

        case keys::blobs_thres_far_inc:
          threshold_far ++;
          if (threshold_far > 255) threshold_far = 255;
          ofLogNotice("DepthBlobsSystem") << "threshold near " << threshold_near << " / far " << threshold_far;
          break;

        case keys::blobs_thres_far_dec:
          threshold_far --;
          if (threshold_far < 0) threshold_far = 0;
          ofLogNotice("DepthBlobsSystem") << "threshold near " << threshold_near << " / far " << threshold_far;
          break;
      }

    };


  private:

    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<BlobsComponent> blobs_m;

    ofxCvContourFinder contourFinder;
    ofxCvGrayscaleImage grey_img_depth_size;
    ofxCvGrayscaleImage grey_img;
    ofxCvGrayscaleImage grey_near;
    ofxCvGrayscaleImage grey_far;

    vector<ofxCvBlob> ofblobs_prev;

    //ofxBlur blur;
    //ofPixels blur_pix;
    //ofxCvGrayscaleImage blur_img;

    //TODO add params
    int blur_size;
    int img_scale;
    int threshold_near;
    int threshold_far;
    //float threshold;

    bool inited;

    void init(int depth_w, int depth_h)
    {
      if (inited) return;

      blur_size = 3;
      img_scale = 6;
      //[0,255]
      threshold_near = 255;
      threshold_far = 209;

      int w = (float)depth_w/img_scale;
      int h = (float)depth_h/img_scale;

      grey_img_depth_size.allocate(depth_w,depth_h);
      grey_img.allocate(w, h);
      grey_near.allocate(w, h);
      grey_far.allocate(w, h);
 
      //int radius = 32, float shape = .2, int passes = 1, float downsample = .5
      //blur.setup(w, h, 10, 0.2, 4, 1.0);
      //blur_img.allocate(w, h);
      //blur_pix.allocate(w, h, 1); //channels

      inited = true;
    };

    void update( uint8_t *depth_pix_grey, int depth_w, int depth_h, BlobsComponent* blobs_data )
    {

      int w = (float)depth_w/img_scale;
      int h = (float)depth_h/img_scale;

      grey_img_depth_size.setFromPixels( depth_pix_grey, depth_w, depth_h );
      //grey_img.resize(w,h);
      grey_img.scaleIntoMe(grey_img_depth_size);

      grey_near = grey_img;
			grey_far = grey_img;
			grey_near.threshold( threshold_near, true );
			grey_far.threshold( threshold_far );
			cvAnd( grey_near.getCvImage(), grey_far.getCvImage(), grey_img.getCvImage(), NULL );
      //grey_img.flagImageChanged();
      grey_img.blurGaussian( blur_size );

      //blur.begin();
      //ofSetColor(255);
      //grey_img.draw(0, 0, w, h);
      //blur.end();
      //blur.getTextureReference().readToPixels(blur_pix);
      //blur_img.setFromPixels(blur_pix.getPixels(), w, h);
      //blur_img.flagImageChanged();
      //contourFinder.findContours( blur_img, 10, (w*h)/3, 20, false ); 

      //img, min area, max area, n considered, holes
      contourFinder.findContours( grey_img, 10, (w*h)/3, 20, false ); 

      //vector<Blob>& blobs = blobs_data->blobs;
      vector<ofPolyline>& blobs = blobs_data->blobs;
      vector<ofxCvBlob>& ofblobs = contourFinder.blobs;
      blobs.clear();
      for ( int i = 0; i < ofblobs.size(); i++ )
      {
        ofPolyline polyblob;

        int prev_i = find_closest( ofblobs_prev, ofblobs[i] );

        if ( prev_i > -1 )
        {
          polyblob = interpolate( ofblobs_prev[prev_i].pts, ofblobs[i].pts );
        }
        else
        {
          polyblob.addVertices( ofblobs[i].pts );
        }

        polyblob.close();

        blobs.push_back( polyblob );
        //blobs.push_back( Blob() );
        //set_blob( w, h, polyblob, blobs[i] ); 
        //set_blob(w, h, ofblobs[i], blobs[i]); 
      }

      ofblobs_prev = contourFinder.blobs;

    }; 

    //void set_blob( int w, int h, const ofPolyline& polyblob, Blob& blob )
    //{
      //blob.centroid = polyblob.getCentroid2D();
      //blob.centroid.x /= w;
      //blob.centroid.y /= h;

      //blob.points = polyblob.getVertices();
      //int plen = blob.points.size();
      //for (int j = 0; j < plen; j++)
      //{
        //blob.points[j].x /= w;
        //blob.points[j].y /= h;
      //}

      //blob.bounds = polyblob.getBoundingBox();
      //blob.bounds.x /= w;
      //blob.bounds.y /= h;
      //blob.bounds.width /= w;
      //blob.bounds.height /= h;

      //blob.area = polyblob.getArea();
      //blob.perimeter = polyblob.getPerimeter();
    //};

    //void set_blob( int w, int h, const ofxCvBlob& ofblob, Blob& blob )
    //{
      //blob.centroid = ofblob.centroid;
      //blob.centroid.x /= w;
      //blob.centroid.y /= h;

      //blob.points = ofblob.pts;
      //int plen = blob.points.size();
      //for (int j = 0; j < plen; j++)
      //{
        //blob.points[j].x /= w;
        //blob.points[j].y /= h;
      //}

      //blob.bounds = ofblob.boundingRect;
      //blob.bounds.x /= w;
      //blob.bounds.y /= h;
      //blob.bounds.width /= w;
      //blob.bounds.height /= h;

      //blob.area = ofblob.area;
      //blob.perimeter = ofblob.length;
    //};

    ofPolyline interpolate( const vector<ofPoint>& src_pts, const vector<ofPoint>& dst_pts )
    {
      float resampled_len = 200.0f;

      ofPolyline src( src_pts );
      src.close();

      ofPolyline dst( dst_pts );
      dst.close();

      ofPolyline dst_resampled = dst.getResampledByCount( resampled_len );
      dst_resampled.close();

      ofPolyline src_resampled = src.getResampledByCount( resampled_len );
      src_resampled.close();

      int len = dst_resampled.size();

      src_resampled.resize( len );
      src_resampled.close();

      for ( int i = 0; i < len; i++ )
        dst_resampled[i].interpolate( src_resampled[i], 0.9999f );

      return dst_resampled;
    };

    int find_closest( const vector<ofxCvBlob>& ofblobs_ls, const ofxCvBlob& ofblob )
    {
      float thres_dist = 300;
      float closest_dist = FLT_MAX;
      int closest_dist_idx;
      for (int i = 0; i < ofblobs_ls.size(); i++)
      {
        float d = ofblob.centroid.distance( ofblobs_ls[i].centroid );
        if ( d < closest_dist ) 
        {
          closest_dist_idx = i;
          closest_dist = d;
        }
      }
      return closest_dist < thres_dist ? closest_dist_idx : -1;
    };

};

