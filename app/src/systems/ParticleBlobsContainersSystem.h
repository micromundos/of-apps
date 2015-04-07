#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"

using namespace artemis;

class ParticleBlobsContainersSystem : public ECSsystem 
{ 

  public:

    ParticleBlobsContainersSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<ParticleBlobsContainersComponent>();
      addComponentType<ParticleSystemComponent>();
    };

    virtual void initialize() 
    {
      p_blob_container_m.init(*world);
      fisica = require_system<FisicaSystem>();
      mesh.setMode(OF_PRIMITIVE_LINES);
    }; 

    virtual void added(Entity &e) 
    {};

    virtual void processEntity(Entity &e) 
    {
      mesh.clear();

      RenderComponent* render_data = require_component<RenderComponent>("output");
      BlobsComponent* blobs_data = require_component<BlobsComponent>("input");
      vector<ofPolyline>& blobs = blobs_data->blobs;

      for ( int i = 0; i < blob_bodies.size(); i++ )
        fisica->b2world()->DestroyBody( blob_bodies[i] );
      blob_bodies.clear();
 
      for( int i = 0; i < blobs.size(); i++ )
      {
        blob_bodies.push_back( make_blob_body( blobs[i], render_data ) );
      }
    }; 

    virtual void renderEntity(Entity &e)
    {
      if ( ! p_blob_container_m.get(e)->render ) 
        return;
      ofPushStyle();
      ofSetLineWidth(1);
      mesh.draw();
      ofPopStyle();
    };

  private:

    ComponentMapper<ParticleBlobsContainersComponent> p_blob_container_m;

    FisicaSystem* fisica; 
    vector<b2Body*> blob_bodies;

    ofVboMesh mesh;

    b2Body* make_blob_body( const ofPolyline& blob, RenderComponent* render_data )
    {
      //TODO try other approaches: 
      //cvApproxPoly
      //blur source img

      float smooth_size = 4.0f;
      float resample_count = 100.0f;
      //float simplify_coef = 0.005f;

      //ofPolyline blob2(blob.getVertices());
      ofPolyline blob2 = blob.getResampledByCount( resample_count ).getSmoothed( smooth_size );
      ////blob2.simplify( simplify_coef );
      ////blob2 = blob2.getSmoothed(smooth_size);
      blob2.close();
      int plen = blob2.size();
      int vlen = plen;
      int step = 1;

      //vector<ofPoint>& points=blob.getVertices();
      //int plen = points.size();
      //int max = 200;
      //int vlen = plen > max ? max : plen;
      //int step = CLAMP( ceil((float)plen/vlen), 1, plen );
      //int step = 10;
      //int vlen = (float)plen/step;

      //ofLogNotice("ParticleBlobsContainersSystem") << " body blob, plen " << plen << ", vlen " << vlen << ", step " << step;

      b2BodyDef bd;
      b2Body* body = fisica->b2world()->CreateBody(&bd);

      b2Vec2* vertices = new b2Vec2[vlen];
      ofVec2f screen_loc, prev_screen_loc;

      //iterate blob2 points
      for ( int vi = 0, pi = 0; vi < vlen; vi++, pi += step )
      {
        screen_loc.set( blob2[pi].x * render_data->width, blob2[pi].y * render_data->height );
        fisica->screen2world( screen_loc, vertices[vi] );

        //debug render 
        //body_blob contours
        if ( pi == 0 ) continue;
        prev_screen_loc.set( blob2[pi-step].x * render_data->width, blob2[pi-step].y * render_data->height );
        mesh.addVertex(prev_screen_loc);
        mesh.addColor(ofFloatColor::yellow);
        mesh.addVertex(screen_loc);
        mesh.addColor(ofFloatColor::yellow);
      }

      //debug render 
      //connect last -> first
      ofVec2f screen_loc_0( blob2[0].x * render_data->width, blob2[0].y * render_data->height );
      mesh.addVertex(screen_loc);
      mesh.addColor(ofFloatColor::yellow);
      mesh.addVertex(screen_loc_0);
      mesh.addColor(ofFloatColor::yellow);

      b2ChainShape loop;
      loop.CreateLoop(vertices, vlen);

      b2FixtureDef fd;
      fd.shape = &loop;
      fd.density = 0.0f;
      fd.restitution = 0.0f;
      fd.friction = 0.2f;;
      body->CreateFixture(&fd);
      //body->CreateFixture(&loop, 0.0f);

      delete[] vertices;
      return body;
    } 

};

