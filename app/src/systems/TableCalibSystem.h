#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "ofxGeom.h"
#include "ofxOpenCv.h"

using namespace artemis;

class TableCalibSystem : public ECSsystem 
{ 

  public:

    TableCalibSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<TableCalibComponent>();
      addComponentType<DepthComponent>();
    };

    virtual void initialize() 
    {
      table_calib_m.init( *world );
      depth_m.init( *world );
      entity = NULL;
    };

    virtual void added(Entity &e) 
    {
      if ( entity != NULL )
      {
        ofLogWarning("TableCalibSystem") << "singleton entity already added";
        return;
      }
      entity = &e;

      DepthComponent* depth_data = depth_m.get(e);

      depth_3d
        .init("glsl/depth_3d.frag", 
            depth_data->width, 
            depth_data->height )
        .on( "update", this, &TableCalibSystem::update_depth_3d );

      height_map
        .init("glsl/height_map.frag", 
            depth_data->width, 
            depth_data->height )
        .on( "update", this, &TableCalibSystem::update_height_map );

      cml_data = require_component<CamaraLucidaComponent>("output");
      ofAddListener( cml_data->cml->render_3d, this, &TableCalibSystem::render_3d );
      //ofAddListener( ofEvents().mousePressed, this, &TableCalibSystem::mousePressed );
      //midx = 0;
    }; 

    virtual void removed(Entity &e) 
    {
      if (cml_data != NULL)
      {
        ofRemoveListener( cml_data->cml->render_3d, this, &TableCalibSystem::render_3d );
        cml_data = NULL;
      }

      depth_3d.off( "update", this, &TableCalibSystem::update_depth_3d );
      height_map.off( "update", this, &TableCalibSystem::update_height_map );

      entity = NULL;

      //ofRemoveListener( ofEvents().mousePressed, this, &TableCalibSystem::mousePressed );
    };

    virtual void processEntity(Entity &e) 
    {
      entity = &e; //save singleton entity
      learn_background_depth_map(e);
      if (calibrate(e)) return;
      if (save(e)) return;
      if (load(e)) return;
    }; 

    virtual void renderEntity(Entity &e)
    {
      //debugging

      TableCalibComponent* table_calib_data = table_calib_m.get(e);

      if ( table_calib_data->render_plane 
          || table_calib_data->render_planes_list 
        ) update_coordmap(); 

      if ( table_calib_data->render_plane )
      {
        ofPushStyle();
        ofSetColor( ofColor::magenta );

        ofVec2f tri2d[3];
        for ( int i = 0; i < 3; i++ )
          depth2screen.dst( cml_data->cml->depth_camera()->project( triangle.vertex(i) ), tri2d[i] );

        for ( int i = 0; i < 3; i++ )
          ofLine( tri2d[i], tri2d[(i+1)%3] );

        ofPopStyle();
      }

      if ( table_calib_data->render_planes_list )
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

    Entity* entity; //singleton entity
    CamaraLucidaComponent* cml_data;

    gpgpu::Process depth_3d;
    gpgpu::Process height_map;

    ofxTriangle triangle; 
    ofxPlane plane;
    vector<ofxTriangle> tris3d;

    //debugging
    vector<ofxTriangle> tris2d;
    ofVboMesh tris2d_mesh;
    ofVboMesh tris3d_mesh;
    CoordMap depth2screen;  

    bool learn_background_depth_map(Entity &e)
    {
      TableCalibComponent* table_calib_data = table_calib_m.get(e);
      DepthComponent* depth_data = depth_m.get(e);

      if ( !table_calib_data->learn_bg
          || !depth_data->dirty )
        return false;

      if ( !table_calib_data->background_depth_map.isAllocated() )
        table_calib_data->background_depth_map.setFromPixels( *(depth_data->f_depth_ofpix_mm) );

      int n = depth_data->f_depth_ofpix_mm->size();
      float* dpix = depth_data->f_depth_ofpix_mm->getPixels();
      float* bgpix = table_calib_data->background_depth_map.getPixels();

      for ( int i = 0; i < n; i++ ) 
      {
        if ( dpix[i] != 0 )
          bgpix[i] = (bgpix[i] + dpix[i]) * 0.5;
      }

      table_calib_data->background_depth_map.update();

      return true;
    };

    bool update_background_height_map(Entity &e)
    {
      TableCalibComponent* table_calib_data = table_calib_m.get(e);

      if ( !table_calib_data->background_depth_map.isAllocated() )
        return false;

      depth_3d
        .set( "depth_map", table_calib_data->background_depth_map.getTextureReference() )
        .update();

      height_map
        .set( "mesh3d", depth_3d.get() )
        .update();

      table_calib_data->background_height_map.setFromPixels( height_map.get_data_pix() );

      return true;
    };

    void update_height_map( ofShader& shader )
    {
      TableCalibComponent* table_calib_data = table_calib_m.get( *entity );
      ofxPlane& p = table_calib_data->plane;
      shader.setUniform4f( "plane", p.a, p.b, p.c, p.d );
    };

    void update_depth_3d( ofShader& shader )
    {
      if ( cml_data == NULL || entity == NULL ) 
        return;

      DepthComponent* depth_data = depth_m.get( *entity );

      shader.setUniform1i("depth_flip", depth_data->flip);

      cml::DepthCamera* depthcam = cml_data->cml->depth_camera();

      shader.setUniform1f("width", depthcam->width);
      shader.setUniform1f("height", depthcam->height);
      shader.setUniform1f("xoff", depthcam->xoff);
      shader.setUniform1f("near", depthcam->near);
      shader.setUniform1f("far", depthcam->far);
      shader.setUniform1f("far_clamp", depthcam->far_clamp);
      shader.setUniform1f("cx", depthcam->cx);
      shader.setUniform1f("cy", depthcam->cy);
      shader.setUniform1f("fx", depthcam->fx);
      shader.setUniform1f("fy", depthcam->fy);
    };

    bool calibrate(Entity &e)
    {
      TableCalibComponent* table_calib_data = table_calib_m.get(e);
      //DepthComponent* depth_data = depth_m.get(e);

      if ( !table_calib_data->calibrate )
          //|| !depth_data->dirty )
        return false;

      //run once
      table_calib_data->calibrate = !table_calib_data->calibrate;  

      find_triangles(e);
      triangle = calc_avg_tri();
      plane = triangle.plane();
      //plane = calc_avg_plane();

      table_calib_data->triangle = triangle;
      table_calib_data->plane = plane;

      //done on learn_background_depth_map
      //table_calib_data->background_depth_map.setFromPixels( *(depth_data->f_depth_ofpix_mm) );

      update_background_height_map(e);

      return true;
    };

    void find_triangles(Entity &e)
    {
      CamaraLucidaSystem* cml_sys = require_system<CamaraLucidaSystem>();

      DepthComponent* depth_data = depth_m.get(e);
      TableCalibComponent* table_calib_data = table_calib_m.get(e);

      int w = depth_data->width;
      int h = depth_data->height;
      uint16_t* depth_pix_mm = depth_data->depth_ofpix_mm->getPixels();

      int nplanes = table_calib_data->planes_num;
      float rstep = table_calib_data->radius_step;
      float astep = table_calib_data->angle_step;

      float r = rstep;
      float ang = astep;

      ofVec2f ctr((float)w/2,(float)h/2);

      tris3d.resize( nplanes );
      tris2d.resize( nplanes );

      for ( int i = 0; i < nplanes; i++ )
      {
        ofVec3f tri3d[3];
        ofVec3f tri2d[3];

        for (int j = 0; j < 3; j++)
        {
          tri2d[j] = ctr + ofVec2f(0,r).rotate( (360./3.)*j + ang ); 
          cml_sys->depth_to_p3(tri2d[j], tri3d[j]);
        } 

        tris3d[i] = ofxTriangle( tri3d[0], tri3d[1], tri3d[2] );
        tris2d[i] = ofxTriangle( tri2d[0], tri2d[1], tri2d[2] );

        r += rstep;
        ang += astep;
      }
    };

    ofxTriangle calc_avg_tri()
    {
      int nplanes = tris3d.size();
      ofVec3f tri[3];

      for ( int i = 0; i < nplanes; i++ )
        for ( int j = 0; j < 3; j++ )
          tri[j] += tris3d[i].vertex(j);
      for ( int j = 0; j < 3; j++ )
        tri[j] /= nplanes;

      ofxTriangle t(tri[0],tri[1],tri[2]);
      return t;
    };

    ofxPlane calc_avg_plane()
    {
      int nplanes = tris3d.size();
      ofVec3f normal;
      ofVec3f ctr;

      for ( int i = 0; i < nplanes; i++ )
      {
        normal += tris3d[i].normal();
        ctr += tris3d[i].centroid();
      }
      normal /= nplanes;
      ctr /= nplanes;

      return ofxPlane( ctr, normal );
    };

    void save_background(Entity &e)
    {
      TableCalibComponent* table_calib_data = table_calib_m.get(e);

      // save background depth map

      ofLogNotice("TableCalibSystem") 
        << "save background_depth_map to " << table_calib_data->filename_background_depth_map;

      cv::Mat background_depth_map = ofxCv::toCv( table_calib_data->background_depth_map );

      cv::FileStorage fs_bg_depth( ofToDataPath(table_calib_data->filename_background_depth_map, false), cv::FileStorage::WRITE );
      fs_bg_depth << "table_background_depth_map" << background_depth_map;

      // save background height map

      ofLogNotice("TableCalibSystem") 
        << "save background_height_map to " << table_calib_data->filename_background_height_map;

      cv::Mat background_height_map = ofxCv::toCv( table_calib_data->background_height_map );

      cv::FileStorage fs_bg_height( ofToDataPath(table_calib_data->filename_background_height_map, false), cv::FileStorage::WRITE );
      fs_bg_height << "table_background_height_map" << background_height_map;
    };

    bool load_background(Entity &e)
    {
      TableCalibComponent* table_calib_data = table_calib_m.get(e);

      // load background depth map

      ofLogNotice("TableCalibSystem") 
        << "load background_depth_map from " << table_calib_data->filename_background_depth_map;

      cv::FileStorage fs_bg_depth( ofToDataPath(table_calib_data->filename_background_depth_map, false), cv::FileStorage::READ );  

      if ( !fs_bg_depth.isOpened() )
      {
        ofLogError("TableCalibSystem") << "failed to load table background_depth_map file " << table_calib_data->filename_background_depth_map;
        return false;
      }

      cv::Mat background_depth_map;
      fs_bg_depth["table_background_depth_map"] >> background_depth_map;
      ofFloatPixels bg_depth;
      ofxCv::toOf( background_depth_map, bg_depth );
      table_calib_data->background_depth_map.setFromPixels( bg_depth );

      // load background height map

      ofLogNotice("TableCalibSystem") 
        << "load background_height_map from " << table_calib_data->filename_background_height_map;

      cv::FileStorage fs_bg_height( ofToDataPath(table_calib_data->filename_background_height_map, false), cv::FileStorage::READ );  

      if ( !fs_bg_height.isOpened() )
      {
        ofLogError("TableCalibSystem") << "failed to load table background_height_map file " << table_calib_data->filename_background_height_map;
        return false;
      }

      cv::Mat background_height_map;
      fs_bg_height["table_background_height_map"] >> background_height_map;
      ofFloatPixels bg_height;
      ofxCv::toOf( background_height_map, bg_height );
      table_calib_data->background_height_map.setFromPixels( bg_height );

      return true;
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

    bool load(Entity &e)
    {
      TableCalibComponent* table_calib_data = table_calib_m.get(e);
      if ( !table_calib_data->load )
        return false;

      table_calib_data->load = false; //once

      return load_background(e) && load_table_plane(e);
    };

    bool save(Entity &e)
    {
      TableCalibComponent* table_calib_data = table_calib_m.get(e);
      if ( !table_calib_data->save )
        return false;

      table_calib_data->save = false; //once

      save_background(e);
      save_table_plane(e); 

      return true;
    };

    void save_table_plane(Entity &e)
    {
      TableCalibComponent* table_calib_data = table_calib_m.get(e);

      ofxTriangle& tri = table_calib_data->triangle;
      ofVec3f v0 = tri.b;
      ofVec3f v1 = tri.b + tri.e0;
      ofVec3f v2 = tri.b + tri.e1;

      ofLogNotice("TableCalibSystem") 
        << "save plane calib to " << table_calib_data->filename_plane << "\n"
        << "v0 " << v0 << "\n"
        << "v1 " << v1 << "\n"
        << "v2 " << v2 << "\n";

      cv::FileStorage fs( ofToDataPath(table_calib_data->filename_plane, false), cv::FileStorage::WRITE ); 

      fs << "vertex_0_x" << v0.x;
      fs << "vertex_0_y" << v0.y;
      fs << "vertex_0_z" << v0.z;

      fs << "vertex_1_x" << v1.x;
      fs << "vertex_1_y" << v1.y;
      fs << "vertex_1_z" << v1.z;

      fs << "vertex_2_x" << v2.x;
      fs << "vertex_2_y" << v2.y;
      fs << "vertex_2_z" << v2.z;
    };

    bool load_table_plane(Entity &e)
    {
      TableCalibComponent* table_calib_data = table_calib_m.get(e);

      cv::FileStorage fs( ofToDataPath(table_calib_data->filename_plane, false), cv::FileStorage::READ ); 

      if ( !fs.isOpened() )
      {
        ofLogError("TableCalibSystem") << "failed to load plane calib file " << table_calib_data->filename_plane;
        return false;
      }

      ofVec3f v0 = ofVec3f( fs["vertex_0_x"], fs["vertex_0_y"], fs["vertex_0_z"] );
      ofVec3f v1 = ofVec3f( fs["vertex_1_x"], fs["vertex_1_y"], fs["vertex_1_z"] );
      ofVec3f v2 = ofVec3f( fs["vertex_2_x"], fs["vertex_2_y"], fs["vertex_2_z"] );

      ofLogNotice("TableCalibSystem")
        << "load plane calib from " << table_calib_data->filename_plane << "\n"
        << "v0 " << v0 << "\n"
        << "v1 " << v1 << "\n"
        << "v2 " << v2 << "\n";

      triangle = ofxTriangle(v0,v1,v2);
      plane = triangle.plane();

      table_calib_data->triangle = triangle;
      table_calib_data->plane = plane;

      return true;
    };

    void render_3d(ofEventArgs &args)
    {
      if ( entity == NULL )
        return;
      _render_3d( *entity );
    };

    void _render_3d(Entity &e)
    {
      TableCalibComponent* table_calib_data = table_calib_m.get(e);

      if ( !table_calib_data->render_plane )
        return;

      ofPushMatrix();
      //ofScale(-1,-1,1);

      ofVec3f ctr = triangle.centroid();

      tris3d_mesh.setMode(OF_PRIMITIVE_TRIANGLES);
      tris3d_mesh.clear();

      ofColor tricols[3] = {
        ofColor::red,
        ofColor::green,
        ofColor::blue
      };

      //render avg tri
      ofVec3f tri[3];
      for ( int i = 0; i < 3; i++ )
        tri[i] = ctr + (ctr - triangle.vertex(i)).normalized() * 500.; 

      for ( int i = 0; i < 3; i++ )
      {
        tris3d_mesh.addColor( ofColor::orange );
        //tris3d_mesh.addVertex(triangle.vertex(i));
        tris3d_mesh.addVertex( tri[i] );
      } 

      ofPushStyle();
      ofSetLineWidth( 3 ); 
      for ( int i = 0; i < 3; i++ )
      {
        ofSetColor( tricols[i] );
        ofLine( tri[i], tri[(i+1)%3] );
      }
      ofPopStyle();

      //render 1st captured tri
      //if ( tris3d.size() > 0 )
      //{
        //ofVec3f tri0[3];
        //for ( int i = 0; i < 3; i++ )
          //tri0[i] = ctr + (ctr - tris3d[0].vertex(i)).normalized() * 500.;

        //for ( int i = 0; i < 3; i++ )
        //{
          //tris3d_mesh.addColor( ofColor::orange );
          //tris3d_mesh.addVertex( tris3d[0].vertex(i) );
          //tris3d_mesh.addVertex( tri0[i] );
        //} 

        //ofPushStyle();
        //ofSetLineWidth( 3 );
        //for ( int i = 0; i < 3; i++ )
        //{
          //ofSetColor( tricols[i] );
          //ofLine( tris3d[0].vertex(i), tris3d[0].vertex((i+1)%3) );
          //ofLine( tri0[i], tri0[(i+1)%3] );
        //}
        //ofPopStyle();
      //}

      //render plane normal
      //ofPushStyle();
      //ofSetColor( ofColor::darkorange );
      //ofSetLineWidth( 3 );
      //ofVec3f n = plane.normal();
      //ofLine( ctr, ctr + n*100. );
      //ofPopStyle();

      tris3d_mesh.draw();

      ofPopMatrix();
    };

    //int midx;
    //ofVec2f mpts[3];
    //ofVec2f mpt;

    //void mousePressed(ofMouseEventArgs &args)
    //{
      //if ( entity == NULL )
        //return;

      ////mpts[midx] = ofVec2f(args.x, args.y);
      ////midx = (midx+1)%3;

      //DepthComponent* depth_data = depth_m.get( entity );

      //mpt.set(args.x, args.y);
      //ofVec3f m3; ofVec2f m2;
      //m2.x = mpt.x/ofGetWidth()*depth_data->width;
      //m2.y = mpt.y/ofGetHeight()*depth_data->height;
      //cml_data->depth_to_p3( depth_data, m2, m3 );
      //float d = plane.distance( m3 );
      //ofLog() << "plane calib:"
        //<< ", triangle = " << triangle.toString() 
        //<< ", plane = " << plane.toString() 
        //<< ", mpt = " << mpt 
        //<< ", m2 = " << m2 
        //<< ", m3 = " << m3
        //<< ", dist = " << d;
    //};

    ComponentMapper<TableCalibComponent> table_calib_m;
    ComponentMapper<DepthComponent> depth_m;

};

