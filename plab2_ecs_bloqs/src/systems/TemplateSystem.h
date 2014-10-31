#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/Components.h"

using namespace artemis;

class TemplateSystem : public ECSsystem 
{ 

  public:

    TemplateSystem() 
    {
      addComponentType<TemplateComponent>();
    };

    virtual void initialize() 
    {
      template_m.init( *world );
    };

    virtual void added(Entity &e) 
    {
      //template_m.get(e)->data;
    };

    virtual void processEntity(Entity &e) 
    {
      //ofLogNotice("TemplateSystem") << "process entity " << e.getId();
      //template_m.get(e)->data;
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      for (int i=0;i<bag.getCount();i++)
        processEntity( *bag.get(i) );
    };

    virtual void render()
    {};

  private:

    ComponentMapper<TemplateComponent> template_m;

};

