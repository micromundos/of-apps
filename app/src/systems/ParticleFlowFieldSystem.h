#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "systems/ParticleSystem.h"
#include "ofEventUtils.h"
#include "CoordMap.h"

using namespace artemis;

class ParticleFlowFieldSystem : public ECSsystem 
{ 

  public:

    ParticleFlowFieldSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<ParticleFlowFieldComponent>();
      addComponentType<ParticleSystemComponent>();
    }; 

    virtual void initialize() 
    {
      particle_flowfield_m.init( *world );
      ps = require_system<ParticleSystem>();
      fisica = require_system<FisicaSystem>();
      
      position_buffer.init("glsl/particles_position.frag",80,60,true);
      texture_positions.allocate(80,60,OF_IMAGE_COLOR_ALPHA);
      
      for(int y=0;y<texture_positions.getHeight();y++)
      {
        for(int x=0;x<texture_positions.getWidth();x++)
        {
          texture_positions.setColor(x,y,ofColor(0.0,0.0,0.0,0.0));
        }
      }
      
      texture_positions.update();
      
     // texture_positions.setUseTexture(true);
      
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      update_screen2ff();
      artemis::EntityProcessingSystem::processEntities( bag );
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("ParticleFlowFieldSystem") << "update";
      
      FlowFieldComponent* ff_data = require_component<FlowFieldComponent>("input");

      //XXX gpu -> cpu
      
    /*  float* field = ff_data->process.get_data();
      if (field == NULL) return;*/

      //if ((ofGetFrameNum()%(60*3))<10) log(field,ff_data->width,ff_data->height);
      
      
      
      
      b2ParticleSystem* b2ps = ps->b2_particles();
      b2Vec2 *          b2position_buffer = b2ps->GetPositionBuffer();
      
      int x = 0;
      int y = 0;
      
 
      
      for(int i=0;i<b2ps->GetParticleCount();i++)
      {
        b2Vec2& loc = b2position_buffer[i];
        ofVec2f ff_loc,screen_loc;
        
        fisica->world2screen(loc,screen_loc);
        screen2ff.dst(screen_loc,ff_loc);
        
        texture_positions.setColor(x,y,ofColor(ff_loc.x,ff_loc.y,0.0,255.0));
        
        if(i == 10)
        {
        //  cout << " 1 = "  << screen_loc.x << " , " << ff_loc.x << "," << ff_loc.y << "\r\n";
        }
        
        x++;
        if(x > texture_positions.getWidth()-1)
        {
          x = 0;
          y++;
        }
      }
      
      
      
      texture_positions.update();
      
      
     
      position_buffer.set("positions",texture_positions.getTextureReference());
      position_buffer.set("flowfield",ff_data->process.get());
      position_buffer.update();
      
      
      field = position_buffer.get_data();
      
 
      if (field == NULL) return;
 /*
      for(int i=0;i<b2ps->GetParticleCount();i++)
      {
          int index = i*4;
          b2Vec2 force = b2Vec2(field[index],field[index+1]);
          b2ps->ParticleApplyForce( i, force );

          if(i == 10){
         //   cout << " 2 = " << field[index] << "," << field[index+1] << "," << field[index+2] << "," << field[index+3]  << "\r\n";
          }
      }
       */
      /*
      
      float* field = ff_data->process.get_data();
       if (field == NULL) return;

      b2ParticleSystem* b2ps = ps->b2_particles();

      int32 n = b2ps->GetParticleCount();
      b2Vec2 *locs = b2ps->GetPositionBuffer(); 

      b2Vec2 force;
      ofVec2f ff_loc, screen_loc;
      
      for (int i = 0; i < n; i++)
      {
        b2Vec2& loc = locs[i]; 
        get_force( field, loc, ff_data, ff_loc, screen_loc, force );
        b2ps->ParticleApplyForce( i, force );
      }
      */

    };

    virtual void renderEntity(Entity &e)
    {
      
      position_buffer.get().draw(0,0);
    
    };

  private:

    ComponentMapper<ParticleFlowFieldComponent> particle_flowfield_m;

    ParticleSystem *ps;
    FisicaSystem* fisica;
  
    gpgpu::Process  position_buffer;
    ofImage         texture_positions;
  
    CoordMap screen2ff;
  float* field;

    void update_screen2ff()
    {
      FlowFieldComponent* ff_data = require_component<FlowFieldComponent>("input");
      RenderComponent* render_data = require_component<RenderComponent>("output");
      screen2ff.set( render_data->width, render_data->height, ff_data->width, ff_data->height );
    };

    void get_force( float* field, const b2Vec2& loc, FlowFieldComponent* ff_data, ofVec2f& ff_loc, ofVec2f& screen_loc, b2Vec2& force )
    {
      fisica->world2screen(loc,screen_loc);
      screen2ff.dst(screen_loc,ff_loc);
      
      int i = ((int)ff_loc.x + (int)ff_loc.y * ff_data->width) * 4; //chann:rgba
      force.Set( field[i], field[i+1] );
    };

    //void log(float* data, int w, int h)
    //{
      //for (int y = 0; y < h; y++)
      //for (int x = 0; x < w; x++)
      //{
        //int i = (x + y * w) * 4;
        //float r = data[i];
        //float g = data[i+1];
        //float b = data[i+2];
        //float a = data[i+3];
        //log_datum(i,x,y,r,g,b,a);
      //}
    //};

    //void log_datum( int i, int x, int y, float r, float g, float b, float a )
    //{
      //ofLog() << "\t" 
        //<< "idx: "
        //<< ofToString(i) 
        //<< "\t xy: "
        //<< ofToString(x) 
        //<< ","
        //<< ofToString(y) 
        //<< "\t data: "
        //<< ofToString(r)
        //<< ","
        //<< ofToString(g)
        //<< ","
        //<< ofToString(b)
        //<< ","
        //<< ofToString(a)
        //<< "";
    //};

};

