#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/Components.h"
#include "ofxOpenCv.h"
//#include "ofxCv.h"

using namespace artemis;

class DepthBlobsSystem : public ECSsystem 
{ 

  public:

    DepthBlobsSystem() 
    {
      addComponentType<DepthComponent>();
      addComponentType<BlobsComponent>();
    };

    virtual void initialize() 
    {
      blobs_m.init( *world );
      depth_m.init( *world ); 
      inited = false; 
    };

    virtual void added(Entity &e) 
    {
      //TODO mejorar
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

      RenderComponent* render_data = component<RenderComponent>("output");
      render( render_data->width, render_data->height ); 
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      for (int i=0;i<bag.getCount();i++)
        processEntity( *bag.get(i) );
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

    //TODO params
    int threshold_near;
    int threshold_far;
    //float threshold;
    //int channels;
    bool inited;

    void init(int w, int h)
    {
      if (inited) return;

      //this->channels = 1;
      //depth_pix.allocate(w, h, channels);
      //depth_pix.set(0); 

      grey_img.allocate(w, h);
      grey_near.allocate(w, h);
      grey_far.allocate(w, h);
      threshold_near = 230;
      threshold_far = 70;

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
      contourFinder.findContours( grey_img, 10, (w*h)/2, 20, false ); 

      //threshold = ofMap( mouseX, 0, ofGetWidth(), 0, 255 );
      //contourFinder.setThreshold( threshold );
      //contourFinder.findContours( ofxCv::toCv( depth_pix ) );

      vector<Blob>& blobs = blobs_data->blobs;
      vector<ofxCvBlob>& ofblobs = contourFinder.blobs;
      blobs.clear();
      for ( int i = 0; i < ofblobs.size(); i++ )
      {
        blobs.push_back( Blob() );
        ofxCvBlob& ofblob = contourFinder.blobs[i];
        blobs[i].centroid = ofblob.centroid;
        blobs[i].points = ofblob.pts;
        blobs[i].bounds = ofblob.boundingRect;
        blobs[i].area = ofblob.area;
        blobs[i].length = ofblob.length;
      }

    };

    void render( int render_width, int render_height )
    {
      ofSetColor(255);
      int w = render_width;
      int h = render_height;
      contourFinder.draw(0, 0, w, w);
      //grey_img.draw(0, 0, w, w);
    };

};

