#pragma once

#include <Artemis/Artemis.h>
#include "ofxECS.h"
#include "Components.h"
#include "bloqs/Attractor.h"
#include "events/AttractorEvents.h"

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

      depth_data = require_component<DepthComponent>("input");

      FlowFieldAttractorsComponent* ff_attr_data = ff_attractors_m.get(e);

      float scale = 0.1; //ff_m.get(e)->scale;
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

      RenderComponent* render_data = require_component<RenderComponent>("output");

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


    void update_flowfield_attractors( ofShader& shader )
    {
      //XXX WARNING keep in sync with 
      //uniform vec2 attractors 
      //in fragment shader
      int max_entities = 10; 
      int max_attrs = max_entities * 2; //attractors = 10 vecs2

      float attr_locs[max_attrs];
      for ( int i = 0; i < max_attrs; i++ )
        attr_locs[i] = 0.0;

      float force_mult[max_entities];
      for ( int i = 0; i < max_entities; i++ )
        force_mult[i] = 0.0;

      int i = 0;
      for ( map<string,Attractor>::iterator it = attractors.begin(); it != attractors.end(); it++ )
      {
        string id = it->first;
        Attractor& attr = it->second;
        attr_locs[ i + 0 ] = attr.loc.x;
        attr_locs[ i + 1 ] = attr.loc.y;
        force_mult[ i ] = attr.force_mult;
        i++;
      } 

      shader.setUniform1i( "attractors_size", attractors.size() );
      shader.setUniform2fv( "attractors_locs", attr_locs, max_entities );
      shader.setUniform1fv( "attractors_force_mult", force_mult, max_entities );
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

