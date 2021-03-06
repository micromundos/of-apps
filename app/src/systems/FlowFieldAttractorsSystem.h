#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "bloqs/Attractor.h"
#include "events/AttractorEvents.h"

#define ATTRS_LEN 10
#define ATTRS_LOCS_LEN ATTRS_LEN*2

using namespace artemis;

class FlowFieldAttractorsSystem : public ECSsystem 
{ 

  public:

    FlowFieldAttractorsSystem(string _id) : ECSsystem(_id)
    {
      addComponentType<FlowFieldAttractorsComponent>();
      addComponentType<FlowFieldComponent>();
    };

    virtual void initialize() 
    {
      ff_attractors_m.init( *world );
      ff_m.init( *world );
      inited = false;
      depth_data = NULL;
    };

    virtual void removed(Entity &e) 
    {
      //TODO ff_attractors_m.get(e).flowfield().dispose();

      ofRemoveListener( AttractorEvents::added, this, &FlowFieldAttractorsSystem::attractor_updated );
      ofRemoveListener( AttractorEvents::updated, this, &FlowFieldAttractorsSystem::attractor_updated );
      ofRemoveListener( AttractorEvents::removed, this, &FlowFieldAttractorsSystem::attractor_removed );

      FlowFieldAttractorsComponent* ff_attr_data = ff_attractors_m.get(e);
      ff_attr_data->flowfield().off( "update", this, &FlowFieldAttractorsSystem::update_flowfield_attractors );

      attractors.clear();
      depth_data = NULL;
      inited = false;
    };

    virtual void added(Entity &e) 
    {
      if ( inited )
      {
        ofLogWarning("FlowFieldAttractorsSystem")
          << "entity already added";
        return;
      }
      inited = true;

      depth_data = component<DepthComponent>("input");

      FlowFieldAttractorsComponent* ff_attr_data = ff_attractors_m.get(e);

      float scale = ff_attr_data->scale;
      int w = depth_data->width * scale;
      int h = depth_data->height * scale;

      ff_attr_data->flowfield()
        .init("glsl/flowfields/flowfield_attractors.frag", w, h )
        .on( "update", this, &FlowFieldAttractorsSystem::update_flowfield_attractors );

      ofAddListener( AttractorEvents::added, this, &FlowFieldAttractorsSystem::attractor_updated );
      ofAddListener( AttractorEvents::updated, this, &FlowFieldAttractorsSystem::attractor_updated );
      ofAddListener( AttractorEvents::removed, this, &FlowFieldAttractorsSystem::attractor_removed );
    }; 

    virtual void processEntity(Entity &e) 
    {
      //if ( ! depth_data->dirty ) return;
      FlowFieldAttractorsComponent* ff_attr_data = ff_attractors_m.get(e);
      ff_attr_data->flowfield()
        .update()
        .update_debug( ff_attr_data->render );
    }; 

    virtual void renderEntity(Entity &e)
    {
      //if ( ! depth_data->dirty ) return;

      FlowFieldAttractorsComponent* ff_attr_data = ff_attractors_m.get(e);

      if ( ! ff_attr_data->render ) return;

      RenderComponent* render_data = component<RenderComponent>("output");

      int rw = render_data->width;
      int rh = render_data->height;

      ff_attr_data->flowfield()
        .render_debug(0,0,rw,rh);
    };

  private:

    DepthComponent* depth_data;
    bool inited;

    map<string, Attractor> attractors; //copies of Attractor

    ComponentMapper<FlowFieldAttractorsComponent> ff_attractors_m; 
    ComponentMapper<FlowFieldComponent> ff_m; 


    //XXX WARNING keep in sync with ATTRS_LEN in fragment shader
    float attractors_locs[ATTRS_LOCS_LEN];
    float attractors_force[ATTRS_LEN];
    float attractors_radius[ATTRS_LEN];


    void update_flowfield_attractors( ofShader& shader )
    { 
      for ( int i = 0; i < ATTRS_LOCS_LEN; i++ )
      {
        attractors_locs[i] = 0.0;
      }

      for ( int i = 0; i < ATTRS_LEN; i++ )
      {
        attractors_force[i] = 0.0;
        attractors_radius[i] = 0.0;
      }

      int i = 0;
      for ( map<string,Attractor>::iterator it = attractors.begin(); it != attractors.end(); it++ )
      {
        string id = it->first;
        Attractor& attr = it->second;
        int ii = i*2; //attr loc
        attractors_locs[ ii ] = attr.loc.x;
        attractors_locs[ ii + 1 ] = attr.loc.y;
        attractors_force[ i ] = attr.force;
        attractors_radius[ i ] = attr.radius;
        i++;
      }  

      shader.setUniform1i( "attractors_size", attractors.size() );
      shader.setUniform2fv( "attractors_locs", attractors_locs, ATTRS_LEN );
      shader.setUniform1fv( "attractors_force", attractors_force, ATTRS_LEN );
      shader.setUniform1fv( "attractors_radius", attractors_radius, ATTRS_LEN );
    };

    void attractor_updated( Attractor& attr )
    {
      attractors[attr.id] = attr;
    };

    void attractor_removed( string& id )
    {
      attractors.erase( id );
    };

};

