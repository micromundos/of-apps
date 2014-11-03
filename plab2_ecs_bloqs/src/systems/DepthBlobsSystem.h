#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/Components.h"
#include "ofxOpenCv.h"
//#include "ofxCv.h"
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

    virtual void render()
    {
      //RenderComponent* render_data = component<RenderComponent>("output");
      //int w = render_data->width;
      //int h = render_data->height;

      //ofSetLineWidth(1);
      //ofSetColor(255);
      ////grey_img.draw(0, 0, w, h);
      //contourFinder.draw(0, 0, w, h);
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

    //ofxCv::ContourFinder contourFinder;
    //ofShortPixels depth_pix; 
    ofxCvContourFinder contourFinder;
    ofxCvGrayscaleImage grey_img;
    ofxCvGrayscaleImage grey_near;
    ofxCvGrayscaleImage grey_far;

    //TODO add params
    int threshold_near;
    int threshold_far;
    //float threshold;
    //int channels;
    bool inited;

    void init(int w, int h)
    {
      if (inited) return;

      //this->channels = 1;
      //depth_pix.allocate(w,h,channels);
      //depth_pix.set(0); 

      grey_img.allocate(w, h);
      grey_near.allocate(w, h);
      grey_far.allocate(w, h);

      //[0,255]
      threshold_near = 255;
      threshold_far = 209;

      //contourFinder.setMinAreaRadius(10);
      //contourFinder.setMaxAreaRadius(150);
      ////contourFinder.setInvert(true); // find black instead of white

      inited = true;
    };

    void update( uint8_t *depth_pix_grey, int w, int h, BlobsComponent* blobs_data )
    {

      //depth_pix.setFromPixels( depth_pix_grey, w, h, channels );

      grey_img.setFromPixels( depth_pix_grey, w, h );
      grey_near = grey_img;
			grey_far = grey_img;
			grey_near.threshold( threshold_near, true );
			grey_far.threshold( threshold_far );
			cvAnd( grey_near.getCvImage(), grey_far.getCvImage(), grey_img.getCvImage(), NULL );
      grey_img.flagImageChanged();
      //img, min area, max area, n considered, holes
      contourFinder.findContours( grey_img, 10, (w*h)/3, 20, false ); 

      //threshold = ofMap( mouseX, 0, ofGetWidth(), 0, 255 );
      //contourFinder.setThreshold( threshold );
      //contourFinder.findContours( ofxCv::toCv( depth_pix ) );

      vector<Blob>& blobs = blobs_data->blobs;
      vector<ofxCvBlob>& ofblobs = contourFinder.blobs;
      blobs.clear();
      for ( int i = 0; i < ofblobs.size(); i++ )
      {
        blobs.push_back( Blob() );
        set_blob( w, h, contourFinder.blobs[i], blobs[i] ); 
      }

    }; 

    void set_blob( int w, int h, const ofxCvBlob& ofblob, Blob& blob )
    {
      blob.centroid = ofblob.centroid;
      blob.centroid.x /= w;
      blob.centroid.y /= h;

      blob.points = ofblob.pts;
      int plen = blob.points.size();
      for (int j = 0; j < plen; j++)
      {
        blob.points[j].x /= w;
        blob.points[j].y /= h;
      }

      blob.bounds = ofblob.boundingRect;
      blob.bounds.x /= w;
      blob.bounds.y /= h;
      blob.bounds.width /= w;
      blob.bounds.height /= h;

      blob.area = ofblob.area;
      blob.length = ofblob.length;
    };

};

