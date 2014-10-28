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
      CamaraLucidaComponent* cml = camara_lucida_m.get(e);
      RenderComponent* render = render_m.get(e);
      //TODO
      cml->tex_width = 1024;
      cml->tex_height = 768;
      render->width = 1024;
      render->height = 768;
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

