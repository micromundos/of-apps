#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxGeom.h"

using namespace artemis;

class PlaneCalibSystem : public ECSsystem 
{ 

  public:

    PlaneCalibSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<PlaneCalibComponent>();
      addComponentType<DepthComponent>();
    };

    virtual void initialize() 
    {
      plane_calib_m.init( *world );
      depth_m.init( *world );
    };

    virtual void added(Entity &e) 
    {}; 

    virtual void processEntity(Entity &e) 
    {
      PlaneCalibComponent* plane_calib_data = plane_calib_m.get(e);

      if (!plane_calib_data->calibrate)
        return;

      DepthComponent* depth_data = depth_m.get(e);

      if ( !depth_data->dirty )
        return;

      plane_calib_data->calibrate = !plane_calib_data->calibrate;  

      vector<ofxTriangle> tris;
      find_triangles( e, tris );
      triang = calc_avg_tri( tris );
      plane_calib_data->plane = triang.plane();
      //plane = calc_avg_plane( tris );
    }; 

    virtual void renderEntity(Entity &e)
    {
      //debugging

      PlaneCalibComponent* plane_calib_data = plane_calib_m.get(e);

      if ( plane_calib_data->render_plane 
          || plane_calib_data->render_planes_list )
      {
        update_coordmap(); 
      }

      if (plane_calib_data->render_plane)
      {
        cml::CamaraLucida* cml = require_component<CamaraLucidaComponent>("output")->cml;

        ofPushStyle();
        ofSetColor( ofColor::magenta );

        ofVec2f* tri2d = new ofVec2f[3];
        for ( int i = 0; i < 3; i++ )
          depth2screen.dst( cml->depth_camera()->project( triang.vertex(i) ), tri2d[i] );

        for ( int i = 0; i < 3; i++ )
          ofLine( tri2d[i], tri2d[(i+1)%3] );
        delete[] tri2d;

        ofPopStyle();
      }

      if (plane_calib_data->render_planes_list)
      {
        tris2d_mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        tris2d_mesh.clear();

        int nplanes = tris2d.size();
        ofFloatColor color;
        ofVec2f p2, screen_loc;

        for ( int i = 0; i < nplanes; i++ )
        {
          float hue = ((float)(i+1)/nplanes);
          color = ofFloatColor::fromHsb(hue,1.,1.);
          for ( int j = 0; j < 3; j++ )
          {
            p2 = ofVec2f( tris2d[i].vertex(j) );
            depth2screen.dst( p2, screen_loc );
            tris2d_mesh.addColor( color );
            tris2d_mesh.addVertex( screen_loc );
          }
        }
        tris2d_mesh.drawWireframe();
      }
    };

  private:

    ofxTriangle triang; 

    //debugging
    vector<ofxTriangle> tris2d;
    ofVboMesh tris2d_mesh;
    CoordMap depth2screen; 

    void find_triangles( Entity &e, vector<ofxTriangle>& planes )
    {
      DepthComponent* depth_data = depth_m.get(e);
      PlaneCalibComponent* plane_calib_data = plane_calib_m.get(e);

      cml::CamaraLucida* cml = require_component<CamaraLucidaComponent>("output")->cml;

      int w = depth_data->width;
      int h = depth_data->height;
      uint16_t* depth_pix_mm = depth_data->depth_pix_mm;

      int nplanes = plane_calib_data->planes_num;
      float rstep = plane_calib_data->radius_step;
      float astep = plane_calib_data->angle_step;

      float r = rstep;
      float ang = 0;
      ofVec2f ctr((float)w/2,(float)h/2);

      planes.resize( nplanes );
      tris2d.resize( nplanes );

      for ( int i = 0; i < nplanes; i++ )
      {
        ofVec3f* tri3d = new ofVec3f[3];
        ofVec3f* tri2d = new ofVec3f[3];

        for (int j = 0; j < 3; j++)
        {
          ofVec2f p2 = ctr + ofVec2f(0,r).rotate( (360./3.)*j ).rotate( ang );

          int ix = p2.y * w + p2.x;
          uint16_t mm = depth_pix_mm[ix];
          float x,y;
          cml->depth_camera()->unproject( p2.x, p2.y, mm, &x, &y );
          tri3d[j] = ofVec3f(x,y,mm); 
          tri2d[j] = ofVec3f(p2);
        } 

        planes[i] = ofxTriangle(tri3d[0],tri3d[1],tri3d[2]);
        tris2d[i] = ofxTriangle(tri2d[0],tri2d[1],tri2d[2]);

        r += rstep;
        ang += astep;

        delete[] tri3d;
        delete[] tri2d;
      }
    };

    ofxTriangle calc_avg_tri( vector<ofxTriangle>& tris )
    {
      int nplanes = tris.size();
      ofVec3f* tri = new ofVec3f[3];

      for ( int i = 0; i < nplanes; i++ )
        for ( int j = 0; j < 3; j++ )
          tri[j] += tris[i].vertex(j);
      for ( int j = 0; j < 3; j++ )
        tri[j] /= nplanes;

      ofxTriangle t(tri[0],tri[1],tri[2]);
      delete[] tri;
      return t;
    };

    ofxPlane calc_avg_plane( vector<ofxTriangle>& tris )
    {
      int nplanes = tris.size();
      ofVec3f normal;
      ofVec3f ctr;

      for ( int i = 0; i < nplanes; i++ )
      {
        normal += tris[i].normal();
        ctr += tris[i].centroid();
      }
      normal /= nplanes;
      ctr /= nplanes;

      return ofxPlane( ctr, normal );
    };

    void update_coordmap()
    {
      DepthComponent* depth_data = require_component<DepthComponent>("input");
      RenderComponent* render_data = require_component<RenderComponent>("output");
      depth2screen.set( 
        depth_data->width, depth_data->height,
        render_data->width, render_data->height 
      );
    };

    ComponentMapper<PlaneCalibComponent> plane_calib_m;
    ComponentMapper<DepthComponent> depth_m;

};

