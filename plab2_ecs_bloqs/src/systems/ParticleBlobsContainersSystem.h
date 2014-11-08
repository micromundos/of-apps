#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/Components.h"
#include "ofxLiquidFun.h"

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
      fisica = system<FisicaSystem>();
      mesh.setMode(OF_PRIMITIVE_LINES);
    };

    virtual void added(Entity &e) 
    {};

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("ParticleBlobsContainersSystem") << "process entity " << e.getId();

      RenderComponent* render_data = component<RenderComponent>("output");
      BlobsComponent* blobs_data = component<BlobsComponent>("input");
      vector<Blob>& blobs = blobs_data->blobs;

      for ( int i = 0; i < blob_bodies.size(); i++ )
        fisica->b2world()->DestroyBody( blob_bodies[i] );
      blob_bodies.clear();
 
      ofPolyline points;
      for( int i = 0; i < blobs.size(); i++ )
      {
        Blob& blob = blobs[i];
        blob_bodies.push_back( make_blob_body( blob, points, render_data ) );
      }

    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      mesh.clear();
      int len = bag.getCount();
      for ( int i = 0; i < len; i++ )
        processEntity( *bag.get(i) );
    };

    virtual void render()
    {
      ofSetLineWidth(1);
      mesh.draw();
    };

  private:

    FisicaSystem* fisica; 
    vector<b2Body*> blob_bodies;

    ofVboMesh mesh;

    b2Body* make_blob_body( const Blob& blob, ofPolyline& points, RenderComponent* render_data )
    {
      //TODO try other approaches: 
      //cvApproxPoly
      //blur source img

      float smooth_size = 4.0f;
      float resample_count = 100.0f;
      //float simplify_coef = 0.005f;

      points.clear();
      points.addVertices( blob.points );
      //points.simplify( simplify_coef );
      //points = points.getSmoothed( smooth_size );
      points = points.getResampledByCount( resample_count ).getSmoothed( smooth_size );
      points.setClosed(true);
      int plen = points.size();
      int vlen = plen;
      int step = 1;

      //const vector<ofPoint>& points = blob.points;
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

      //iterate points
      for ( int vi = 0, pi = 0; 
          vi < vlen; 
          vi++, pi += step )
      {
        screen_loc.set( points[pi].x * render_data->width, points[pi].y * render_data->height );
        fisica->screen2world( screen_loc, vertices[vi] );

        //debug render 
        //body_blob contours
        if ( pi == 0 ) continue;
        prev_screen_loc.set( points[pi-step].x * render_data->width, points[pi-step].y * render_data->height );
        mesh.addVertex(prev_screen_loc);
        mesh.addColor(ofFloatColor::yellow);
        mesh.addVertex(screen_loc);
        mesh.addColor(ofFloatColor::yellow);
      }

      //debug render 
      //connect last -> first
      ofVec2f screen_loc_0( points[0].x * render_data->width, points[0].y * render_data->height );
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

