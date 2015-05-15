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

      float* field = ff_data->field;
      //float* field = ff_data->process.get_data();
      //ofTexture* field = ff_data->field; 
      if (field == NULL) return;

      //if ((ofGetFrameNum()%(60*3))<10) log(field,ff_data->width,ff_data->height);

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

    };

    virtual void renderEntity(Entity &e)
    {};

  private:

    ComponentMapper<ParticleFlowFieldComponent> particle_flowfield_m;

    ParticleSystem *ps;
    FisicaSystem* fisica;

    CoordMap screen2ff;

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

