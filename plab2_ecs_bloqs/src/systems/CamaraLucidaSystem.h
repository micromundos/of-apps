#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/Components.h"
#include "ofxCamaraLucida.h"

using namespace artemis;

class CamaraLucidaSystem : public ECSsystem 
{ 

  public:

    CamaraLucidaSystem() 
    {
      addComponentType<CamaraLucidaComponent>();
      addComponentType<RenderComponent>();
    };

    virtual void initialize() 
    {
      camara_lucida_m.init( *world );
      render_m.init( *world );
    };

    virtual void added(Entity &e) 
    {
      //cml::CamaraLucida* cml = component<CamaraLucidaComponent>("output")->cml;
      //float w = cml->tex_width();
      //float h = cml->tex_height();
      //render_m.get(e)->update(w,h);
    };

    virtual void processEntity(Entity &e) 
    {
      DepthComponent* depth = component<DepthComponent>("input");

      if ( depth->dirty )
        component<CamaraLucidaComponent>("output")->cml->update( depth->depth_pix_mm );
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      for (int i=0;i<bag.getCount();i++)
        processEntity( *bag.get(i) );
    };

  private:

    ComponentMapper<CamaraLucidaComponent> camara_lucida_m;
    ComponentMapper<RenderComponent> render_m;

};

