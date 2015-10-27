#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "systems/ParticleSystem.h"
#include "ofEventUtils.h"
#include "CoordMap.h"
#include "ofxTimeMeasurements.h"

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
      ps = system<ParticleSystem>();
      fisica = system<FisicaSystem>();
      ff_data = NULL;
      render_data = NULL;
    };

    virtual void removed(Entity &e) 
    { 
      ff_data = NULL;
      render_data = NULL;
    };

    virtual void added(Entity &e) 
    {
      ff_data = component<FlowFieldComponent>("ff");
      render_data = component<RenderComponent>("output");
    };

    virtual void processEntity(Entity &e) 
    {
      update_screen2ff();

      ParticleFlowFieldComponent* pff_data = particle_flowfield_m.get(e);

      int ff_width = ff_data->flowfield().width();

      float* field = NULL;
      //XXX gpu -> cpu
      TS_START("ParticleFlowFieldSystem #gpu->cpu");
      field = ff_data->flowfield().get_data();
      TS_STOP("ParticleFlowFieldSystem #gpu->cpu");
      if (field == NULL) 
      {
        ofLogWarning("ParticleFlowFieldSystem")
          << "flow field input process has no data";
        return;
      }

      TS_START("ParticleFlowFieldSystem");

      float maxforce = pff_data->maxforce;

      b2ParticleSystem* b2ps = ps->b2_particles();

      int32 n = b2ps->GetParticleCount();
      b2Vec2 *locs = b2ps->GetPositionBuffer(); 

      b2Vec2 force;
      ofVec2f ff_loc, screen_loc;
      for (int i = 0; i < n; i++)
      {
        b2Vec2& loc = locs[i]; 

        fisica->world2screen( loc, screen_loc );
        screen2ff.dst( screen_loc, ff_loc );

        int fi = ((int)ff_loc.x + (int)ff_loc.y * ff_width) * 4; //chann:rgba
        force.Set( field[fi], field[fi+1] );

        if ( maxforce > 0 )
        {
          float len = force.Normalize();
          force *= len > maxforce ? maxforce : len;
          //cout << i << ": force len " << len << ", max " << maxforce << endl;
        }

        b2ps->ParticleApplyForce( i, force );
      }

      TS_STOP("ParticleFlowFieldSystem");
    };

    virtual void renderEntity(Entity &e)
    {};

  private:

    FlowFieldComponent* ff_data;
    RenderComponent* render_data;

    ComponentMapper<ParticleFlowFieldComponent> particle_flowfield_m;

    ParticleSystem *ps;
    FisicaSystem* fisica;

    CoordMap screen2ff;

    void update_screen2ff()
    {
      int ff_width = ff_data->flowfield().width();
      int ff_height = ff_data->flowfield().height();
      screen2ff.set( render_data->width, render_data->height, ff_width, ff_height ); 
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

