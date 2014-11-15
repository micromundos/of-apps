#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "components/Components.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "keys.h"

using namespace artemis;

class DepthBlobsTrackerSystem : public ECSsystem 
{ 

  public:

    DepthBlobsTrackerSystem(string _id) : ECSsystem(_id)
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
      //ofAddListener( ofEvents().keyPressed, this, &DepthBlobsTrackerSystem::keyPressed );
    };

    virtual void added(Entity &e) 
    {
      //TODO mejorar init
      init( depth_m.get(e) );
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("DepthBlobsTrackerSystem") << "process entity " << e.getId();

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
      BlobsComponent* blobs_data = blobs_m.get(e);

      if ( blobs_data->render_depth_filtered )
      {
        RenderComponent* render_data = component<RenderComponent>("output");
        int w = render_data->width;
        int h = render_data->height;

        ofPushStyle();
        ofSetColor(255);
        dpix_img.draw(0, 0, w, h);
        ofPopStyle();
      }

      if ( blobs_data->render_contour_finder )
      {
        ofPushStyle();
        ofSetLineWidth(1);
        contourFinder.draw();
        ofPopStyle();
      }
    };


  private:

    ComponentMapper<DepthComponent> depth_m;
    ComponentMapper<BlobsComponent> blobs_m;

    ofPixels dpix; 
    ofPixels dpix_orig_size; 
    ofPixels dpix_near; 
    ofPixels dpix_far; 
    ofxCv::ContourFinder contourFinder; 
		map<unsigned int, ofPolyline> ofblobs_prev;

    //debug render
    ofImage dpix_img; 

    float resampled_len;
    float interpolation_coef;
    float img_scale;
    int blur_size;
    int channels;
    float blobs_threshold;

    bool inited;

    void init( DepthComponent* depth )
    {
      if (inited) return;

      int depth_w = depth->width;
      int depth_h = depth->height;

      //TODO add params
      resampled_len = 100.0f;
      interpolation_coef = 0.06f;
      blur_size = 5;
      img_scale = 0.15;
      blobs_threshold = 15;
      channels = 1;

      //[0,255]
      //threshold_near = 255;
      //threshold_far = 209; 

      int w = (float)depth_w * img_scale;
      int h = (float)depth_h * img_scale;

      dpix_orig_size.allocate( depth_w, depth_h, channels );
      dpix.allocate( w, h, channels );
      dpix_near.allocate( w, h, channels );
      dpix_far.allocate( w, h, channels );

      dpix_orig_size.set(0);
      dpix.set(0);
      dpix_near.set(0);
      dpix_far.set(0);

      //debug render
      dpix_img.allocate(w, h, OF_IMAGE_GRAYSCALE);

      contourFinder.setMinAreaRadius( 10 );
      contourFinder.setMaxAreaRadius( (w*h)/3 );
      //contourFinder.setInvert(true); // find black instead of white

      ofxCv::RectTracker& tracker = contourFinder.getTracker();
      tracker.setPersistence( 20 );
      tracker.setMaximumDistance( 100 );

      inited = true;
    };

    void update( uint8_t *depth_pix_grey, int depth_w, int depth_h, BlobsComponent* blobs_data )
    {

      int w = (float)depth_w * img_scale;
      int h = (float)depth_h * img_scale;

      dpix_orig_size.setFromPixels( depth_pix_grey, depth_w, depth_h, channels );

      ofxCv::resize( dpix_orig_size, dpix, img_scale, img_scale );
      ofxCv::copy( dpix, dpix_near );
      ofxCv::copy( dpix, dpix_far );
      ofxCv::threshold( dpix_far, blobs_data->threshold_far );
      ofxCv::threshold( dpix_near, blobs_data->threshold_near );
      ofxCv::add(dpix_near, dpix_far, dpix);
      ofxCv::blur( dpix, blur_size );

      if ( blobs_data->render_depth_filtered )
      {
        dpix_img.setFromPixels( dpix );
      }

      contourFinder.setThreshold( blobs_threshold );
      contourFinder.findContours( dpix );

      vector<ofPolyline>& blobs = blobs_data->blobs;
      const vector<ofPolyline>& ofblobs = contourFinder.getPolylines();

      ofxCv::RectTracker& tracker = contourFinder.getTracker();

      int len = contourFinder.size();

      blobs.clear();
      for (unsigned int i = 0; i < len; i++)
      {
        unsigned int label = contourFinder.getLabel(i);

        ofPolyline& prev = ofblobs_prev[label];
        if ( tracker.existsPrevious(label) ) 
        {
          ofPolyline interpolated = interpolate( prev, ofblobs[i] );
          blobs.push_back( interpolated );
          prev.clear();
          prev.addVertices( interpolated.getVertices() );
        }
        else
        {
          ofPolyline resampled = ofblobs[i].getResampledByCount( resampled_len );
          blobs.push_back( resampled );
          prev.clear();
          prev.addVertices(resampled.getVertices());
        }

        normalize_blob( w, h, blobs[i] );
      }

    }; 

    void normalize_blob( int w, int h, ofPolyline& blob )
    {
      int plen = blob.size();
      for (int j = 0; j < plen; j++)
      {
        blob[j].x /= w;
        blob[j].y /= h;
      }
    };

    ofPolyline interpolate( const ofPolyline& src, const ofPolyline& dst )
    {

      ofPolyline dst1 = dst.getResampledByCount( resampled_len );
      dst1.close();

      //already resampled
      ofPolyline src1( src.getVertices() );
      //ofPolyline src1 = src.getResampledByCount( resampled_len );
      src1.close();

      int len = min( dst1.size(), src1.size() );
      dst1.resize( len );
      dst1.close();
      src1.resize( len );
      src1.close();

      for ( int i = 0; i < len; i++ )
        src1[i].interpolate( dst1[i], interpolation_coef );

      return src1;
    };

};

