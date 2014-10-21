#pragma once

#include <Artemis/Artemis.h>
#include "ecs/ECSsystem.h"
#include "components/TemplateComponent.h"

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

    virtual void processEntity(Entity &e) 
    {
      template_m.get(e)->data += world->getDelta();
    };

    virtual void processEntities( ImmutableBag<Entity*>& bag ) 
    {
      for ( int i = 0; i < bag.getCount(); i++ )
      {
        processEntity( *bag.get(i) );
      }
    };

  private:

    ComponentMapper<TemplateComponent> template_m;

};

