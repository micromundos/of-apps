#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"

using namespace artemis;

class BlobsSystem : public ECSsystem 
{ 

  public:

    BlobsSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<BlobsComponent>();
      addComponentType<DepthSmoothingComponent>();
      addComponentType<DepthComponent>();
    };

    virtual void initialize() 
    {
      blobs_m.init( *world );
      smooth_m.init( *world );
      depth_m.init( *world ); 
      inited = false; 
    };

    virtual void added(Entity &e) 
    {
      if (inited) return;
      inited = true;

      ofPixels& input = get_input(e);

      resampled_len = 100.0f;
      interpolation_coef = 0.06f;
      blobs_threshold = 15;

      int w = input.getWidth();
      int h = input.getHeight();

      contourFinder.setMinAreaRadius( 10 );
      contourFinder.setMaxAreaRadius( (w*h)/3 );
      //contourFinder.setInvert(true); // find black instead of white

      ofxCv::RectTracker& tracker = contourFinder.getTracker();
      tracker.setPersistence( 20 );
      tracker.setMaximumDistance( 100 );

      inited = true;
    };

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth_data = depth_m.get(e);
      BlobsComponent* blobs_data = blobs_m.get(e);

      if ( !depth_data->dirty )
        return;

      ofPixels& input = get_input(e);

      int w = input.getWidth();
      int h = input.getHeight();

      contourFinder.setThreshold( blobs_threshold );
      contourFinder.findContours( input );

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

    virtual void renderEntity(Entity &e)
    {
      BlobsComponent* blobs_data = blobs_m.get(e);

      if ( blobs_data->render )
      {
        ofPushStyle();
        ofSetLineWidth(1);
        contourFinder.draw();
        ofPopStyle();
      }
    };

  private:

    ofxCv::ContourFinder contourFinder; 
		map<unsigned int, ofPolyline> ofblobs_prev;

    float blobs_threshold;
    float resampled_len;
    float interpolation_coef;

    bool inited;

    ComponentMapper<BlobsComponent> blobs_m;
    ComponentMapper<DepthSmoothingComponent> smooth_m;
    ComponentMapper<DepthComponent> depth_m;

    ofPixels& get_input(Entity &e)
    {
      return smooth_m.get(e)->output;
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

