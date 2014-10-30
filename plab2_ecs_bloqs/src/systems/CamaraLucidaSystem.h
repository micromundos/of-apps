#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/Components.h"

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
      CamaraLucidaComponent* cml_data = camara_lucida_m.get(e);
      RenderComponent* render_data = render_m.get(e);
      //TODO
      int w = 1014;
      int h = 768;
      cml_data->tex_width = w;
      cml_data->tex_height = h;
      render_data->width = w;
      render_data->height = h;
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("CamaraLucidaSystem") << "process entity " << e.getId();
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

